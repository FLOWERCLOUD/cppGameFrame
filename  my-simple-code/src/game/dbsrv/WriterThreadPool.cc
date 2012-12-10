
#include <game/dbsrv/WriterThreadPool.h>

#include <game/dbsrv/WriterThread.h>

WriterThreadPool::WriterThreadPool(int threadnum)
{
	threads_.resize(threadnum);

	for (int i = 0; i < threadnum; i++)
	{
		threads_[i] = new WriterThread(i + 1);
	}
}

WriterThreadPool::~WriterThreadPool()
{
	int threadnum = threads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		delete threads_[i];
	}
}

void WriterThreadPool::start()
{
	int threadnum = threads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		threads_[i]->start();
	}
}

void WriterThreadPool::stop()
{
	int threadnum = threads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		threads_[i]->stop();
	}
}

void WriterThreadPool::push(struct WriterThreadParam& param)
{
	static int nextThreadId = 0;
	int threadnum = threads_.size();
	nextThreadId++;
	if (nextThreadId >= threadnum)
	{
		nextThreadId = 0;
	}

	threads_[nextThreadId]->push(param);
}

void WriterThreadPool::ping()
{
	struct WriterThreadParam param;
	param.Type = WRITERTHREAD_PING;

	int threadnum = threads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		threads_[i]->push(param);
	}
}

