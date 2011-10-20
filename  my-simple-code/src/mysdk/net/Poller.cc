
#include <mysdk/net/Poller.h>

#include <mysdk/net/EventLoop.h>

using namespace mysdk;
using namespace mysdk::net;

Poller::Poller(EventLoop* eventLoop):
		loop_(eventLoop)
{
}

Poller::~Poller()
{
}

