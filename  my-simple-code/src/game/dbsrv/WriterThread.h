
#ifndef GAME_WRITERTHREAD_H_
#define GAME_WRITERTHREAD_H_

#include <mysdk/base/Thread.h>
#include <mysdk/base/BlockingQueue.h>

#include <game/dbsrv/mysql/MySQLConnection.h>

typedef enum tagWriterThreadMsgType
{
	WRITERTHREAD_PING = 1,
	WRITERTHREAD_CMD = 2,
	WRITERTHREAD_STOP = 3
} WriterThreadMsgType;

#define REDISKEY_MAXLEN 29
typedef struct tagWriterThreadParam
{
	WriterThreadMsgType Type;
	void* sql;
	unsigned long length;
	char key[REDISKEY_MAXLEN + 1];
}WriterThreadParam;

class WriterThread
{
public:
	WriterThread(int id);
	~WriterThread();

public:
	void start();
	void stop();

	void push(WriterThreadParam& param);
private:
	void threadHandler();
	void handler(WriterThreadParam& param);
private:
	int id_;
	mysdk::BlockingQueue<WriterThreadParam> queue_;
	mysdk::Thread thread_;
	MySQLConnection mysql_;
};


#endif /* WRITERTHREAD_H_ */
