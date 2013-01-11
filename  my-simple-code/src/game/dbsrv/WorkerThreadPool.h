
#ifndef WORKERTHREADPOOL_H_
#define WORKERTHREADPOOL_H_

#include <vector>

#include <mysdk/base/Singleton.h>

class DBSrv;
class WorkerThread;
class WorkThreadPool
{
public:
	WorkThreadPool();
	~WorkThreadPool();

	void start(DBSrv* srv, int threadnum);
	void stop();

	int push(struct ThreadParam& param);

	void ping();
private:
	std::vector<WorkerThread* > asyncThreads_;
};

#define sWorkThreadPool mysdk::Singleton<WorkThreadPool>::instance()

#endif /* WORKERTHREADPOOL_H_ */
