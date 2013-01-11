
#include "LogThread.h"
#include <mysdk/base/Timestamp.h>

#include <stdarg.h>

static const char* LogLevelName[NUM_LOG_LEVELS] =
{
		"TRACE",
		"DEBUF",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL",
};

const char* PerLogFile[NUM_LOG_FILES] =
{
		"session",
		"normal",
		"recover"
};

LogThread::LogThread():
	thread_(std::tr1::bind(&LogThread::threadHandler, this), "LogThread"),
	logLevel_(0),
	bstart_(false)
{
	for (int i = 0; i < NUM_LOG_FILES; i++)
	{
		files_[i] = NULL;
		lines_[i] = 0;
	}

	const char* srvpath = getenv("DBSRVPATH");
	char path[100];
	snprintf(path, 99, "%s/log", srvpath);
	logpath_ =path;
}

LogThread::~LogThread()
{
	for (int i = 0; i < NUM_LOG_FILES; i++)
	{
		if(files_[i])
		{
			fclose(files_[i]);
			files_[i] = NULL;
			renameLog((LogFileType)i);
		}
	}
}


void LogThread::start(std::string srvName, int logLevel)
{
	bstart_ = true;
	srvName_ = srvName;
	logLevel_ = logLevel;
	thread_.start();
}

void LogThread::stop()
{
	bstart_ = false;
	LogThreadItem item;
	item.Type = LOGTHREAD_STOP;
	queue_.put(item);
	thread_.join();

}

void LogThread::threadHandler()
{
	while (true)
	{
		LogThreadItem item = queue_.take();
		if (item.Type == LOGTHREAD_STOP)
		{
			break;
		}
		else if (item.Type == LOGTHREAD_CMD)
		{
			LogFileType fileType = item.fileType;
			createFile(fileType);
			if (files_[fileType])
			{
				fprintf(files_[fileType], "%s\n", item.log);
				fflush(files_[fileType]);

				rotateLog(fileType);
			}

			DeleteBuf(item.log);
		}
	}
}

static void makeTimeToString(char* buf, int bufsize)
{
	time_t seconds = time(NULL);
	struct tm tm_time;
	localtime_r(&seconds, &tm_time);
	snprintf(buf, bufsize, "%02d%02d_%02d%02d%02d",
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec);
}

std::string LogThread::getFileName(LogFileType type)
{
	char filename[100];
	snprintf(filename, sizeof(filename) - 1, "%s%s.log+",  srvName_.c_str(), PerLogFile[type]);
	return filename;
}

void LogThread::createFile(LogFileType type)
{
	if (!files_[type])
	{
		files_[type] = fopen(getFileName(type).c_str(), "a");
	}
	return;
}

#include "config/ConfigMgr.h"
void LogThread::rotateLog(LogFileType type)
{
	static  int maxlines = sConfigMgr.MainConfig.GetIntDefault("log", "maxlines", 1000);
	lines_[type]++;
	if (lines_[type] >= maxlines)
	{
		fclose(files_[type]);
		files_[type] = NULL;
		renameLog(type);
		createFile(type);
		lines_[type] = 0;
	}
}

void LogThread::renameLog(LogFileType type)
{
	char newpath[100];
	char date[32];
	makeTimeToString(date, sizeof(date) - 1);
	snprintf(newpath, sizeof(newpath), "%s/%s%s_%s.log", logpath_.c_str(), srvName_.c_str(), PerLogFile[type], date);

	if(rename(getFileName(type).c_str(), newpath) != 0)
	{
		fprintf(stderr, "LogThread::renameLog, rename error, errno: %d\n", errno);
	}
}
#include <string.h>
void LogThread::log(int level, const char* file, int line, const char* funcname, const char* msg, ...)
{
	if(!bstart_)return;
	if (level < logLevel_) return;

	const char* path_sep_pos = strrchr(file, '/');
	const char* basename = (path_sep_pos != NULL) ? path_sep_pos + 1 : file;

	static const int maxbufsize = 2 * 1024;
	char message[maxbufsize];
	snprintf(message, maxbufsize, "%s %s %s-%s:%d %s",
			mysdk::Timestamp::now().toFormattedString().c_str(),
			LogLevelName[level],
			msg,
			basename,
			line,
			funcname);

	char* log = NewBuf(maxbufsize);
	va_list ap;
	va_start(ap, msg);
	vsnprintf(log, maxbufsize, message, ap);
	va_end(ap);

	LogThreadItem item;
	item.Type = LOGTHREAD_CMD;
	item.fileType = NORMAL;
	item.log = log;
	queue_.put(item);
}

void LogThread::rawLog(const char* msg, ...)
{
	if(!bstart_)return;

	static const int maxbufsize = 1024;
	char* log = NewBuf(maxbufsize);
	va_list ap;
	va_start(ap, msg);
	vsnprintf(log, maxbufsize, msg, ap);
	va_end(ap);

	LogThreadItem item;
	item.Type = LOGTHREAD_CMD;
	item.fileType = RECOVER;
	item.log = log;
	queue_.put(item);
}
