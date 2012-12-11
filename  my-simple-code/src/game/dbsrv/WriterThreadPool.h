
#ifndef WRITERTHREADPOOL_H_
#define WRITERTHREADPOOL_H_


#include <vector>

class WriterThread;
class WriterThreadPool
{
public:
	WriterThreadPool(int threadnum);
	~WriterThreadPool();

	void start();
	void stop();

	void push(struct WriterThreadParam& param);
	void push(int threadId, struct WriterThreadParam& param);
	int getThreadNum();
	void ping();
private:
	std::vector<WriterThread* > threads_;
	int threadnum_;
};

#endif /* WRITERTHREADPOOL_H_ */
