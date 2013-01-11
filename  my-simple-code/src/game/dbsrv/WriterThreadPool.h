
#ifndef WRITERTHREADPOOL_H_
#define WRITERTHREADPOOL_H_


#include <vector>

#include <game/dbsrv/WriterThread.h>
#include <mysdk/base/Singleton.h>

class WriterThreadPool
{
public:
	WriterThreadPool();
	~WriterThreadPool();

	void start(int threadnum);
	void stop();

	void push(WriterThreadParam& param);
	void push(int threadId, WriterThreadParam& param);
	int getThreadNum();
	void ping();

	std::string getQueueInfo();
public:
	char* NewSqlBuf(size_t size)
	{
		return new char[size];
	}
	void DeleteSqlBuf(char* sqlbuf)
	{
		delete[]sqlbuf;
	}
private:
	std::vector<WriterThread* > threads_;
	int threadnum_;
};

#define sWriterThreadPool mysdk::Singleton<WriterThreadPool>::instance()

#endif /* WRITERTHREADPOOL_H_ */
