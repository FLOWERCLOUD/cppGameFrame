
#include <mysdk/net/Poller.h>
#include <mysdk/net/poller/EPollPoller.h>

#include <stdlib.h>

using namespace mysdk::net;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
	if (::getenv("MYSDK_USE_POLL"))
	{
		return new EPollPoller(loop);
	}
	return new EPollPoller(loop);
}

