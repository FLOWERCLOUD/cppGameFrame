
#include <game/dbsrv/config/ConfigMgr.h>
#include <game/dbsrv/DBSrv.h>
#include <game/dbsrv/version.h>

#include <mysdk/base/Daemon.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

#include <stdarg.h>
#include <pwd.h>

static void show_help()
{
    printf("dbsrv vesion %.2f\n", DBSRVVERSION);
    printf("-p <num>      TCP port number to listen on (default: 5007)\n"
           "-l <ip_addr>  interface to listen on (default: INADDR_ANY, all addresses)\n"
           "-d            run as a daemon\n"
    		"-f            config file path name\n"
           "-r            maximize core file limit\n"
           "-u <username> assume identity of <username> (only when run as root)\n"
           "-c <num>      max simultaneous connections (default: 1024)\n"
           "-v            print vesion \n"
           "-h            print this help and exit\n"
    		);
    return;
}

static void show_version()
{
	printf("dbsrv vesion %.2f -- by zjx (zhanjunxiong@126.com) \n", DBSRVVERSION);
}

void vperror(const char *fmt, ...)
{
    int old_errno = errno;
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    if (vsnprintf(buf, sizeof(buf), fmt, ap) == -1)
    {
        buf[sizeof(buf) - 1] = '\0';
    }
    va_end(ap);

    errno = old_errno;

    perror(buf);
}


static void save_pid(const pid_t pid, const char *pid_file)
{
    FILE *fp;
    if (pid_file == NULL)
        return;

    if ((fp = fopen(pid_file, "w")) == NULL)
    {
        vperror("Could not open the pid file %s for writing", pid_file);
        return;
    }

    fprintf(fp,"%ld\n", static_cast<long>(pid));
    if (fclose(fp) == -1)
    {
        vperror("Could not close the pid file %s", pid_file);
        return;
    }
}

static void remove_pidfile(const char *pid_file)
{
  if (pid_file == NULL)
      return;

  if (unlink(pid_file) != 0)
  {
      vperror("Could not remove the pid file %s", pid_file);
  }
}

int main(int argc, char **argv)
{
	// 设置要打印日志等级
	//Logger::setLogLevel(Logger::INFO);

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	int defalutPort = 5007;  //默认端口
	std::string configFile("config/def.conf");
	int maxconns = 1024;

	int o;
	int maxcore = 0;
	char *username = NULL;
	char *pid_file = NULL;
	bool do_daemonize = false;
	while(-1 != (o = getopt(argc, argv,
			"h"
			 "p:"  // TCP port number to listen on
			"f:"   // config file path name
			"r"   // maximize core file limit
			"u:" // user identity to run as
			"c:" // max simultaneous connections
			"d" //  run as a daemon
			"P:"
			"v")))
	{
		switch(o)
		{
		case 'v':
			show_version();
			return 0;
		case 'h':
			show_help();
			return 0;
		case 'p':
			defalutPort = atoi(optarg);
			break;
		case 'f':
			configFile = optarg;
			break;
		case 'r':
			maxcore = 1;
			break;
        case 'u':
            username = optarg;
            break;
        case 'c':
        	maxconns = atoi(optarg);
        	break;
        case 'd':
            do_daemonize = true;
            break;
        case 'P':
            pid_file = optarg;
            break;
		default:
			show_help();
			return -1;
		}
	}

	std::string fullConfigFile;
	const char *configPath = getenv("dbsrv_configpath");
	if (!configPath)
	{
		char path[FILENAME_MAX];
		if (!(getcwd(path,
				FILENAME_MAX)))
		{
			printf("dbsrv getcwd error! --- vesion: .%2f\n", DBSRVVERSION);
			return -1;
		}
		std::string tmp(path);
		fullConfigFile = tmp  + "/" + configFile;
	}
	else
	{
		std::string tmp(configPath);
		fullConfigFile =  tmp + "/" + configFile;
	}

	LOG_INFO << "============ start dbsrv ============ version "  << DBSRVVERSION;
	if(sConfigMgr.MainConfig.SetSource(fullConfigFile.c_str(), true))
	{
		LOG_INFO << "Config Passed without errors. --- fullConfigFile: " << fullConfigFile;
	}
	else
	{
		LOG_ERROR << "Config Encountered one or more errors. --- fullConfigFile: " << fullConfigFile;
		return -1;
	}

	struct rlimit rlim;
    if (maxcore != 0)
    {
        struct rlimit rlim_new;
        /*
         * First try raising to infinity; if that fails, try bringing
         * the soft limit to the hard.
         */
        if (getrlimit(RLIMIT_CORE, &rlim) == 0)
        {
            rlim_new.rlim_max = -1;
            rlim_new.rlim_cur = rlim_new.rlim_max;
            if (setrlimit(RLIMIT_CORE, &rlim_new)!= 0)
            {
                /* failed. try raising just to the old max */
                rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
                setrlimit(RLIMIT_CORE, &rlim_new);
            }
        }
        /*
         * getrlimit again to see what we ended up with. Only fail if
         * the soft limit ends up 0, because then no core files will be
         * created at all.
         */

        if ((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_cur == 0)
        {
            fprintf(stderr, "failed to ensure corefile creation\n");
           	return -1;
        }
    }

    /*
     * If needed, increase rlimits to allow as many connections
     * as needed.
     */

    if (getrlimit(RLIMIT_NOFILE, &rlim) != 0)
    {
        fprintf(stderr, "failed to getrlimit number of files\n");
        return -1;
    }
    else
    {
        unsigned int maxfiles = maxconns;
        if (rlim.rlim_cur < maxfiles)
            rlim.rlim_cur = maxfiles;
        if (rlim.rlim_max < rlim.rlim_cur)
            rlim.rlim_max = rlim.rlim_cur;
        if (setrlimit(RLIMIT_NOFILE, &rlim) != 0)
        {
            fprintf(stderr, "failed to set rlimit for open files. Try running as root or requesting smaller maxconns value.\n");
            return -1;
        }
    }

    struct passwd* pw;
    /* lose root privileges if we have them */
    if (getuid() == 0 || geteuid() == 0)
    {
        if (username == 0 || *username == '\0')
        {
            fprintf(stderr, "can't run as root without the -u switch\n");
            return -1;
        }
        if ((pw = getpwnam(username)) == 0)
        {
            fprintf(stderr, "can't find the user %s to switch to\n", username);
            return -1;
        }
        if (setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0)
        {
            fprintf(stderr, "failed to assume identity of user %s\n", username);
            return -1;
        }
    }

    /* daemonize if requested */
    /* if we want to ensure our ability to dump core, don't chdir to / */
    if (do_daemonize)
    {
    	/*
        if (sigignore(SIGHUP) == -1)
        {
            perror("Failed to ignore SIGHUP");
        }
        */
        if (daemonize(maxcore, 0) == -1)
        {
            fprintf(stderr, "failed to daemon() in order to daemonize\n");
            return -1;
        }
    }

    if (do_daemonize)
    {
    	save_pid(getpid(), pid_file);
    }

	uint16 srvPort = static_cast<uint16>(sConfigMgr.MainConfig.GetIntDefault("net", "port", defalutPort));
	LOG_INFO << "dbsrv listen port[" << srvPort << "]";
	EventLoop loop;
	InetAddress listenAddr(srvPort);
	DBSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();

	LOG_INFO << "============  stop dbsrv ============ version "  << DBSRVVERSION;

    /* remove the PID file if we're a daemon */
    if (do_daemonize)
    {
    	remove_pidfile(pid_file);
    }

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
