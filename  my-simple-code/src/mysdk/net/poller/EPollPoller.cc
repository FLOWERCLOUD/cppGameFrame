
#include <mysdk/net/poller/EPollPoller.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/Session.h>
#include <mysdk/net/Poller.h>

#include <assert.h>
#include <errno.h>
#include <sys/epoll.h>

#include <string.h> // bzero

using namespace mysdk;
using namespace mysdk::net;

EPollPoller::EPollPoller(EventLoop* eventLoop):
		Poller(eventLoop),
		epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
		events_(kInitEventListSize)
{
	if (epollfd_ < 0)
	{
		LOG_SYSFATAL << "EPollPoller::EPollPoller";
	}
}

EPollPoller::~EPollPoller()
{
	::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, SessionList* activeSessions)
{
	int numEvents = ::epoll_wait(epollfd_,
									&*events_.begin(),
									static_cast<int>(events_.size()),
									timeoutMs);
	Timestamp now(Timestamp::now());
	if (numEvents > 0)
	{
		LOG_TRACE << numEvents << " events happended";
		fillActiveSessions(numEvents, activeSessions);
		if (implicit_cast<size_t>(numEvents) == events_.size())
		{
			events_.resize( events_.size() << 1);
		}
	}
	else if (numEvents == 0)
	{
		LOG_TRACE << " nothing happended";
	}
	else
	{
		LOG_SYSERR << "EPollPoller::poll()";
	}
	return now;
}

void EPollPoller::fillActiveSessions(int numEvents, SessionList* activeSessions) const
{
	assert(implicit_cast<size_t>(numEvents) <= events_.size());
	for (int i = 0; i < numEvents; ++i)
	{
		Session* session = static_cast<Session*>(events_[i].data.ptr);
		int mask = 0;
		if (events_[i].events & EPOLLIN) mask |= Session::kReadEvent;
		if (events_[i].events & EPOLLOUT) mask |= Session::kWriteEvent;
		if (events_[i].events & EPOLLERR) mask |= Session::kErrorEvent;
		if (events_[i].events & EPOLLHUP) mask |= Session::kHupEvent;
		session->setFireEvents(mask);
		activeSessions->push_back(session);
	}
}

void EPollPoller::addEvent(Session* session, int mask)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = session->events() == Session::kNoneEvent ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    mask |= session->events();
    if (mask & Session::kReadEvent) event.events |= EPOLLIN;
    if (mask & Session::kWriteEvent) event.events |= EPOLLOUT;
    event.data.ptr = session;

    if (::epoll_ctl(epollfd_, op, session->fd(), &event) < 0)
    {
    	LOG_SYSFATAL << "epoll_ctl op=" << op;
    }
    LOG_TRACE  << " add epoll " << session->fd();
}

void EPollPoller::delEvent(Session* session,  int delmask)
{
	int mask = session->events() & (~delmask);
	struct epoll_event event;
	bzero(&event, sizeof(event));
	if (mask & Session::kReadEvent) event.events |= EPOLLIN;
	if (mask & Session::kWriteEvent) event.events |= EPOLLOUT;
	event.data.ptr = session;
	if (mask != Session::kNoneEvent)
	{
		::epoll_ctl(epollfd_, EPOLL_CTL_MOD, session->fd(), &event);
        LOG_TRACE  << " mod epoll " << session->fd();
	}
	else
	{
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, session->fd(), &event);
        LOG_TRACE  << " del epoll " << session->fd();
	}
}

