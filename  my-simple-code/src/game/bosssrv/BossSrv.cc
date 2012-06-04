
#include <game/bosssrv/BossSrv.h>

#include <game/bosssrv/config/ConfigMgr.h>
#include <game/bosssrv/protocol/GameProtocol.h>
#include <game/bosssrv/version.h>

#include <string.h>
#include <signal.h>

//#define HAVE_BACKTRACE

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif

#ifdef HAVE_BACKTRACE
#pragma GCC diagnostic ignored "-Wold-style-cast"
static void *getMcontextEip(ucontext_t *uc) {
#if defined(__FreeBSD__)
    return (void*) uc->uc_mcontext.mc_eip;
#elif defined(__dietlibc__)
    return (void*) uc->uc_mcontext.eip;
#elif defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
  #if __x86_64__
    return (void*) uc->uc_mcontext->__ss.__rip;
  #elif __i386__
    return (void*) uc->uc_mcontext->__ss.__eip;
  #else
    return (void*) uc->uc_mcontext->__ss.__srr0;
  #endif
#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
  #if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
    return (void*) uc->uc_mcontext->__ss.__rip;
  #else
    return (void*) uc->uc_mcontext->__ss.__eip;
  #endif
#elif defined(__i386__)
    return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
    return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
    return (void*) uc->uc_mcontext.sc_ip;
#else
    return NULL;
#endif
}

void bugReportStart()
{
	LOG_WARN << "=== BossSrv BUG REPORT START: Cut & paste starting from here ===";
}

void bugReportEnd()
{
	  LOG_WARN <<  "=== BossSrv BUG REPORT END. Make sure to include from START to END. ===";
}

static void sigsegvHandler(int sig, siginfo_t *info, void *secret)
{
    ucontext_t *uc = (ucontext_t*) secret;
    struct sigaction act;

    bugReportStart();
    LOG_WARN << "    BossSrv "  << BOSSSRVVERSION << " crashed by signal: " << sig;

	void *trace[100];
    /* Generate the stack trace */
    int trace_size = backtrace(trace, 100);

    /* overwrite sigaction with caller's address */
    if (getMcontextEip(uc) != NULL) {
        trace[1] = getMcontextEip(uc);
    }
    char **messages = backtrace_symbols(trace, trace_size);
    LOG_WARN << "--- STACK TRACE" ;
    for (int i=1; i<trace_size; ++i)
    {
    	 LOG_WARN << messages[i];
    }

    bugReportEnd();

    /* Make sure we exit with the right signal at the end. So for instance
     * the core will be dumped if enabled. */
    sigemptyset (&act.sa_mask);
    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
     * is used. Otherwise, sa_handler is used */
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;
    sigaction (sig, &act, NULL);
    kill(getpid(),sig);
}
#pragma GCC diagnostic error "-Wold-style-cast"
#endif /* HAVE_BACKTRACE */

static bool shutdown_bossSrv = false;
static void sigtermHandler(int sig)
{
    LOG_WARN << "Received SIGTERM, scheduling shutdown...";
    shutdown_bossSrv = true;
}

static void reReadConfHandler(int sig)
{
    LOG_WARN << "Received SIGUSR1, reReadConfHandler...";

}

static void openActHandler(int sig)
{
    LOG_WARN << "Received SIGUSR2, openActHandler...";
}

void setupSignalHandlers(void)
{
    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
#ifdef HAVE_BACKTRACE
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
#endif

    act.sa_flags = 0;
    act.sa_handler = reReadConfHandler;
    sigaction(SIGUSR1,&act,NULL);
    act.sa_handler = openActHandler;
    sigaction(SIGUSR2,&act,NULL);
    return;
}


BossSrv::BossSrv(EventLoop* loop, InetAddress& serverAddr):
	open_(false),
	haveAward_(false),
	bossId_(0),
	bossHp_(0),
	bossElem_(0),
	initBossHp_(1),
	bossHurtMgr_(this),
	phpThread_(std::tr1::bind(&BossSrv::phpThreadHandler, this), "phpThread"),
	codec_(
				std::tr1::bind(&BossSrv::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3)),
	loop_(loop),
	server_(loop, serverAddr, "BossSrv")
{
	server_.setConnectionCallback(std::tr1::bind(&BossSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
														&codec_,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));

    loop->runEvery(1.0, std::tr1::bind(&BossSrv::tickMe, this));
	LOG_DEBUG << "============ BossSrv::BossSrv serverAddr: "<<  server_.hostport()
							<< " ================";
}

BossSrv::~BossSrv()
{
	LOG_DEBUG << "============ BossSrv::~BossSrv serverAddr: " <<  server_.hostport()
							<< " ================";
}

// return [nBegin, nEnd]
static inline int32 getRandomBetween(int32 nBegin, int32 nEnd)
{
	return nBegin + static_cast<int32>(random() % (nEnd - nBegin + 1) );
}

void BossSrv::start()
{
	setupSignalHandlers();
	phpThread_.start();
	bossId_ = sConfigMgr.MainConfig.GetIntDefault("boss", "bossid", 10283);
	initBossHp_ = bossHp_ = sConfigMgr.MainConfig.GetIntDefault("boss", "bosshp", 100000110);
	assert(initBossHp_ > 0);
	bossElem_ = getRandomBetween(0, 12);
	server_.start();
}

void BossSrv::stop()
{
	tellPhpActOver();

	ThreadParam x;
	x.cmd = 3;
	queue_.put(x);
	phpThread_.join();
}

void BossSrv::tickMe()
{
	if (shutdown_bossSrv)
	{
		loop_->quit();
	}
}


void BossSrv::onConnectionCallback(mysdk::net::TcpConnection* pCon)
{
	if (pCon)
	{
		LOG_TRACE << pCon->peerAddress().toHostPort() << " -> "
				<< pCon->localAddress().toHostPort() << " is"
				<< (pCon->connected() ? "UP" : "DOWN");


		if (pCon->connected())
		{
			// 初始化活动数据
			PacketBase op(game::OP_BOSSSRV_UPDATEDATA, 1);
			op.putInt32(bossId_);
			op.putInt32(bossHp_);
			op.putInt32(bossElem_);
			send(pCon, op);
		}
	}
}

void BossSrv::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 cmd = pb.getOP();
	if (cmd == game::OP_BOSSSRV_UPDATEDATA)
	{
		uint32 param = pb.getParam();
		if (param == 0)
		{
			if (bossHp_ <= 0) return;

			uint32 uid = pb.getInt32();
			char username[521];
			if (!pb.getUTF(username, 512)) return;

			uint32 hurtValue = pb.getInt32();
			uint32 flag = pb.getInt32();
			bossHurtMgr_.addHurt(uid, hurtValue, username);
			tellPhpPlayerHurt(uid, hurtValue, username, flag);

			bossHp_ -= hurtValue;
			PacketBase op(game::OP_BOSSSRV_UPDATEDATA, 2);
			op.putInt32(bossHp_ < 0 ? 0 : bossHp_);
			send(pCon, op);

			if (bossHp_ <= 0)
			{
				bossHp_ = 0;
				tellPhpActOver();
			}
		}
		else if (param == 3) // 排行榜记录
		{
			bossHurtMgr_.serializeTop(pb);
			send(pCon, pb);
		}
	}
}


void BossSrv::send(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	codec_.send(pCon, pb);
}

void BossSrv::broadMsg(PacketBase& pb)
{
	TcpServer::TcpConnectionArray& conArray = server_.getTcpConnectionArray();
	TcpServer::TcpConnectionArray::size_type size = conArray.size();
	for (TcpServer::TcpConnectionArray::size_type i = 0; i < size; i++)
	{
		  TcpConnection* conn = conArray[i];
		  if (conn)
		  {
			  send(conn, pb);
		  }
	}
}

#include <game/LoongBgSrv/php/htmlclient.h>
CHtmlClient htmlClient;

void BossSrv::phpThreadHandler()
{
	LOG_INFO << "====== phpThreadHandler STARTING ";
	static const std::string phpAddr(sConfigMgr.MainConfig.GetStringDefault("phpAddr", "gamelog", "115.238.55.103/gamelog/feedback.php"));
	static const std::string mailphpAddr(sConfigMgr.MainConfig.GetStringDefault("phpAddr", "newmail", "115.238.55.103/newmail/send_display.php"));
	char buf[1024];
	while (true)
	{
		ThreadParam param = queue_.take();
		if (param.cmd == 1)
		{
			snprintf(buf, 1023, "%s?%s",
				phpAddr.c_str(),
				param.param);

			LOG_INFO << "PHP: " << buf;
		}
		else if (param.cmd == 2)
		{
			snprintf(buf, 1023, "%s?%s",
					mailphpAddr.c_str(),
				param.param);

			LOG_INFO << "PHP: " << buf;
		}
		else
		{
			break;
		}
		delete[] param.param;
		htmlClient.loadUrl(buf);
	}
	LOG_INFO << "======= phpThreadHandler END ";
}

#include <game/bosssrv/php/urlEncode.h>

void BossSrv::tellPhpPlayerHurt(uint32 uid, uint32 hurtvalue, char* username, uint32 flag)
{
	ThreadParam param;
	param.cmd = 1;

	char enName[128];
	if (!ctool::URLEncode(username, enName, 128)) return;

	char* buf = new char[1024];
	snprintf(buf, 1023, "ex_name=jihad&uid=%d&hurt=%d&uname=%s&flag=%d", uid, hurtvalue, enName, flag);
	param.param = buf;
	queue_.put(param);
}

#include <sys/time.h>
#include <time.h>

void BossSrv::tellPhpActOver()
{
	if (haveAward_) return;

	static const std::string mailcontent(sConfigMgr.MainConfig.GetStringDefault("mail",
			"mailcontent",
			"uid=10000&sname=叮叮博士&friends=%d&sysid=10000&adjunct=%s&isadjunct=0&background=1&time=%d&title=%s 讨伐令奖励&content=恭喜你 %s 对噬天大帝造成的伤害达到%.2f%%，超过0.05%%，获得以下奖励一份。&button=ok"));

	static const std::string mailcontentaward(sConfigMgr.MainConfig.GetStringDefault("mail", "mailcontentaward", "3:100013:1|3:100034:1"));

	static const std::string topmailcontent(sConfigMgr.MainConfig.GetStringDefault("mail",
			"topmailcontent",
			"uid=10000&sname=叮叮博士&friends=%d&sysid=10000&adjunct=%s&isadjunct=0&background=1&time=%d&title=%s 讨伐令奖励&content=恭喜你 %s对噬天大帝造成的伤害值排名第%d，获得一下奖励一份。&button=ok"));

	//static const std::string topmailcontentaward(sConfigMgr.MainConfig.GetStringDefault("mail", "topmailcontentaward", "3:100013:1|3:103002:1"));

	std::string topaward[5];
	topaward[0] = "3:100151:1|3:103002:1";
	topaward[1] = "3:100152:1|3:103002:1";
	topaward[2] = "3:100153:1|3:103002:1";
	topaward[3] = "3:100154:1|3:103002:1";
	topaward[4] = "3:100155:1|3:103002:1";

	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm * tm = localtime(&tv.tv_sec);

	char date[20];
	snprintf(date, 20, "%4d.%02d.%02d", tm->tm_year + 1900 , tm->tm_mon + 1, tm->tm_mday);

	BossHurtMgr::TopPlayerVectorT& topPlayerVector = bossHurtMgr_.getTopPlayerVector();
	size_t topNum = topPlayerVector.size();
	for (size_t i = 0; i < topNum; i++)
	{
		Player* player = topPlayerVector[i];
		if (player)
		{
			int index = getRandomBetween(0, 4);
			ThreadParam param;
			param.cmd = 2;
			char* buf = new char[1024];
			snprintf(buf, 1023, topmailcontent.c_str(),  player->uid, topaward[index].c_str(), tv.tv_sec, date, date, i + 1);
			param.param = buf;
			queue_.put(param);
		}
	}

	BossHurtMgr::PlayerHurtMapT& playerHurtMap = bossHurtMgr_.getPlayerHurtMap();
	BossHurtMgr::PlayerHurtMapT::iterator iter;
	uint32 num = 0;
	for(iter = playerHurtMap.begin(); iter != playerHurtMap.end(); iter++)
	{
		num++;
		float hurtvalue =static_cast<float>(iter->second);

		float per = hurtvalue / static_cast<float> (initBossHp_);
		LOG_DEBUG << " uid: " << iter->first << " hurtvalue: " << hurtvalue << " per: " << per;
		static float sAwardPer = sConfigMgr.MainConfig.GetFloatDefault("mail", "awardper", 0.05f);
		if (per >= sAwardPer)
		{
			ThreadParam param;
			param.cmd = 2;
			char* buf = new char[1024];
			snprintf(buf, 1023, mailcontent.c_str(),  iter->first, mailcontentaward.c_str(), tv.tv_sec, date, date, per);
			param.param = buf;
			queue_.put(param);
		}
	}

	haveAward_ = true;
	LOG_INFO << "============== SUM: " << num;
}
