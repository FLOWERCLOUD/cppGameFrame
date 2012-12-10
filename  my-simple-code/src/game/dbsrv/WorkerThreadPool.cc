
#include <game/dbsrv/WorkerThreadPool.h>

#include <game/dbsrv/WorkerThread.h>

WorkThreadPool::WorkThreadPool(DBSrv* srv, int threadnum)
{
	asyncThreads_.resize(threadnum);

	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i] = new WorkerThread(srv, i + 1);
	}
}

WorkThreadPool::~WorkThreadPool()
{
	int threadnum = asyncThreads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		delete asyncThreads_[i];
	}
}

void WorkThreadPool::start()
{
	int threadnum = asyncThreads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->start();
	}
}

void WorkThreadPool::stop()
{
	int threadnum = asyncThreads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->stop();
	}
}

void WorkThreadPool::push(struct ThreadParam& param)
{
	static int nextThreadId = 0;
	int threadnum = asyncThreads_.size();
	nextThreadId++;
	if (nextThreadId >= threadnum)
	{
		nextThreadId = 0;
	}

	asyncThreads_[nextThreadId]->push(param);
}

void WorkThreadPool::ping()
{
	struct ThreadParam param;
	param.Type = PING;

	int threadnum = asyncThreads_.size();
	for (int i = 0; i < threadnum; i++)
	{
		asyncThreads_[i]->push(param);
	}
}
