
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
	void ping();
private:
	std::vector<WriterThread* > threads_;
};

#endif /* WRITERTHREADPOOL_H_ */
