
#ifndef GAME_LOGTHREAD_H_
#define GAME_LOGTHREAD_H_

#include <mysdk/base/BlockingQueue.h>
#include <mysdk/base/Thread.h>
#include <mysdk/base/Singleton.h>

#include <string>
#include <stdio.h>

typedef enum tagLogLevel
{
	TRACE,
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL,
	NUM_LOG_LEVELS,
} LogLevel;


typedef enum tagLogFileType
{
	SESSION = 0, //会话日志
	NORMAL,
	RECOVER,
	NUM_LOG_FILES
}LogFileType;

typedef enum tagLogThreadMsgType
{
	LOGTHREAD_PING = 1,
	LOGTHREAD_CMD = 2,
	LOGTHREAD_STOP = 3
} LogThreadMsgType;

typedef struct tagLogThreadItem
{
	LogThreadMsgType Type;
	LogFileType fileType;
	char* log;
}LogThreadItem;

class LogThread
{
public:
	LogThread();
	~LogThread();

public:
	char* NewBuf(size_t size)
	{
		return new char[size];
	}
private:
	void DeleteBuf(char* buf)
	{
		delete[] buf;
	}
public:
	void start(std::string srvName, int logLevel);
	void stop();

	void log(int level, const char* file, int line, const char* fncname, const char* msg, ...);
	void rawLog(const char* msg, ...);
private:
	void threadHandler();
private:
	void createFile(LogFileType type);
	std::string getFileName(LogFileType type);
	void rotateLog(LogFileType type);
	void renameLog(LogFileType type);
private:
	mysdk::BlockingQueue<LogThreadItem> queue_;
	mysdk::Thread thread_;
	FILE* files_[NUM_LOG_FILES];
	int lines_[NUM_LOG_FILES];
	std::string srvName_;
	std::string logpath_;
	int logLevel_;
	bool bstart_;
};

#define sLogThread mysdk::Singleton<LogThread>::instance()

#define LOGEX_TRACE( msg, ... ) sLogThread.log(TRACE, __FILE__, __LINE__, __FUNCTION__, msg, ##__VA_ARGS__ )
#define LOGEX_DEBUG( msg, ... ) sLogThread.log(DEBUG, __FILE__, __LINE__, __FUNCTION__ , msg, ##__VA_ARGS__ )
#define LOGEX_INFO( msg, ... ) sLogThread.log(INFO, __FILE__, __LINE__, __FUNCTION__, msg, ##__VA_ARGS__ )
#define LOGEX_WARN( msg, ... ) sLogThread.log(WARN, __FILE__, __LINE__, __FUNCTION__, msg, ##__VA_ARGS__ )
#define LOGEX_ERROR( msg, ... ) sLogThread.log(ERROR, __FILE__, __LINE__, __FUNCTION__, msg, ##__VA_ARGS__ )
#define LOGEX_RAW(msg, ...) sLogThread.rawLog(msg, ##__VA_ARGS__)

#endif
