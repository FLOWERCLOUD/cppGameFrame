
#ifndef MYSDK_NET_POLLER_EPOLLPOLLER_H
#define MYSDK_NET_POLLER_EPOLLPOLLER_H

#include <mysdk/net/Poller.h>

#include <mysdk/base/Timestamp.h>

#include <vector>

#include <sys/epoll.h>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Session;
	class EPollPoller: public Poller
	{
	public:
		EPollPoller(EventLoop* loop);
		virtual ~EPollPoller();

		virtual Timestamp poll(int timeoutMs, SessionList* activeSessions);
		void addEvent(Session* session, int mask);
		void delEvent(Session* session, int delmask);
	private:
		static const int kInitEventListSize = 16;

		void fillActiveSessions(int numEvents, SessionList* activeSessions) const;

		typedef std::vector<struct epoll_event> EventList;

		int epollfd_;
		EventList events_;
	};

}
}

#endif
