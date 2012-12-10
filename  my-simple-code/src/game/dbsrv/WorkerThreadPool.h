
#ifndef WORKERTHREADPOOL_H_
#define WORKERTHREADPOOL_H_

#include <vector>

class DBSrv;
class WorkerThread;
class WorkThreadPool
{
public:
	WorkThreadPool(DBSrv* srv, int threadnum);
	~WorkThreadPool();

	void start();
	void stop();

	void push(struct ThreadParam& param);

	void ping();
private:
	std::vector<WorkerThread* > asyncThreads_;
};

#endif /* WORKERTHREADPOOL_H_ */
