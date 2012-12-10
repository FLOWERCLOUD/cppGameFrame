

#ifndef GAME_CLIENTTHREAD_H_
#define GAME_CLIENTTHREAD_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>
#include <mysdk/base/Thread.h>
#include <mysdk/base/BlockingQueue.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#include "DBClient.h"

using namespace mysdk;
using namespace mysdk::net;

class ClientThread
{
public:
	ClientThread(InetAddress addr);
	~ClientThread();

public:
	void start();
	void stop();

	void push(char* cmd);
private:
	void threadHandler();
private:
	BlockingQueue<char*> queue_;
	Thread thread_;
	EventLoop loop_;
	DBClient client_;
};

#endif /* WORKER_H_ */
