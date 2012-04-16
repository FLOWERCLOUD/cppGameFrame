
#include <mysdk/net/EventLoop.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/Session.h>
#include <mysdk/net/Poller.h>

#include <signal.h>
#include <sys/eventfd.h>

using namespace mysdk;
using namespace mysdk::net;

namespace
{
	const int kPollTimeMs = 10000;
	int createEventfd()
	{
		int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (evtfd < 0)
		{
			LOG_SYSERR << "Failed in eventfd";
			abort();
		}
		return evtfd;
	}

#pragma GCC diagnostic ignored "-Wold-style-cast"
	class IgnoreSigPipe
	{
	public:
		IgnoreSigPipe()
		{
			::signal(SIGPIPE, SIG_IGN);
			LOG_TRACE << "Ignore SIGPIPE";
		}
	};
#pragma GCC diagnostic error "-Wold-style-cast"

	IgnoreSigPipe initObj;
}

EventLoop::EventLoop():
		looping_(false),
		quit_(false),
		callingPendingFunctors_(false),
		wakeupFd_(createEventfd()),
		pPoller_(Poller::newDefaultPoller(this))
{
	pTimerQueue_ = new TimerQueue(this);

	pWakeupSession_ = new Session(this, wakeupFd_);
	pWakeupSession_->setReadCallback(std::tr1::bind(&EventLoop::handleRead, this));
	pWakeupSession_->enableReading();
}

EventLoop::~EventLoop()
{
	::close(wakeupFd_);
	if (pTimerQueue_)
	{
		delete pTimerQueue_;
	}

	if (pWakeupSession_)
	{
		delete pWakeupSession_;
	}

	if (pPoller_)
	{
		delete pPoller_;
	}
}

void EventLoop::loop()
{
	looping_ = true;
	quit_ = false;
	LOG_TRACE << "EventLoop " << this << " start looping ";
	while(!quit_)
	{
		activeSessions_.clear();
		pollReturnTime_ = pPoller_->poll(kPollTimeMs, &activeSessions_);
	    if (Logger::logLevel() <= Logger::TRACE)
	    {
	    	//printActiveSessions();
	    }

		eventHanding_ = true;
		for (SessionList::iterator it = activeSessions_.begin(); it != activeSessions_.end(); ++it)
		{
			(*it)->handleEvent(pollReturnTime_);
		}
		eventHanding_ = false;

	    doPendingFunctors();
	}
	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
	wakeup();
}

void EventLoop::wakeup()
{
	uint64 one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
	if ( n != sizeof(one))
	{
		LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
	return pTimerQueue_->schedule(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return pTimerQueue_->schedule(cb, time, interval);
}

void EventLoop::addEvent(Session* session, const int mask)
{
	pPoller_->addEvent(session, mask);
}

void EventLoop::delEvent(Session* session, const int delmask)
{
	pPoller_->delEvent(session, delmask);
}

////
void EventLoop::handleRead()
{
	LOG_TRACE << "EventLoop::handleRead";

	uint64 one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_ERROR << "EventLoop::wakeup() reads " << n << " bytes instead of 8";
	}
}

void EventLoop::doPendingFunctors()
{
	 std::vector<Functor> functors;
	 callingPendingFunctors_ = true;

	{
	    MutexLockGuard lock(mutex_);
	    functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}

	callingPendingFunctors_ = false;
}

void EventLoop::queueInLoop(const Functor& cb)
{
	  {
		  MutexLockGuard lock(mutex_);
		  pendingFunctors_.push_back(cb);
	  }

	  if (callingPendingFunctors_)
	  {
		  wakeup();
	  }
}

void EventLoop::printActiveSessions() const
{
  for (SessionList::const_iterator it = activeSessions_.begin();
      it != activeSessions_.end(); ++it)
  {
    const Session* session = *it;
    LOG_TRACE << "{" << session->reventsToString() << "} ";
  }
}
