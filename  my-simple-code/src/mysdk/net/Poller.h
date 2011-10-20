
#ifndef MYSDK_NET_POLLER_H
#define MYSDK_NET_POLLER_H

#include <mysdk/base/Common.h>

#include <mysdk/base/Timestamp.h>

#include <vector>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Session;
	class Poller
	{
	public:
		typedef std::vector<Session*> SessionList;

	public:
		Poller(EventLoop* loop);
		virtual ~Poller();

		virtual Timestamp poll(int timeoutMs, SessionList* activeSessions) = 0;
		virtual void addEvent(Session* session, int mask) = 0;
		virtual void delEvent(Session* session, int delmask) = 0;

		static Poller* newDefaultPoller(EventLoop* loop);
	private:
		EventLoop* loop_;
	private:
		DISALLOW_COPY_AND_ASSIGN(Poller);
	};
}

}

#endif
