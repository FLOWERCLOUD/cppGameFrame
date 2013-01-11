
#include <game/dbsrv/WorkerThreadPool.h>

#include <game/dbsrv/WorkerThread.h>

WorkThreadPool::WorkThreadPool()
{
}

WorkThreadPool::~WorkThreadPool()
{
	size_t threadnum = asyncThreads_.size();
	for (size_t i = 0; i < threadnum; i++)
	{
		delete asyncThreads_[i];
	}
}

void WorkThreadPool::start(DBSrv* srv, int threadnum)
{
	asyncThreads_.resize(threadnum);
	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i] = new WorkerThread(srv, i + 1);
	}

	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->start();
	}
}

void WorkThreadPool::stop()
{
	size_t threadnum = asyncThreads_.size();
	for (size_t i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->stop();
	}
}

int WorkThreadPool::push(struct ThreadParam& param)
{
	static size_t nextThreadId = 0;
	size_t threadnum = asyncThreads_.size();
	nextThreadId++;
	if (nextThreadId >= threadnum)
	{
		nextThreadId = 0;
	}

	asyncThreads_[nextThreadId]->push(param);
	return nextThreadId;
}

void WorkThreadPool::ping()
{
	struct ThreadParam param;
	param.Type = PING;

	size_t threadnum = asyncThreads_.size();
	for (size_t i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->push(param);
	}
}
