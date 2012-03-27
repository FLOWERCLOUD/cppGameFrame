
#include <mysdk/net/TimerQueue.h>

#include <mysdk/base/Logging.h>
#include <mysdk/base/Mutex.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/Timer.h>
#include <mysdk/net/TimerId.h>

#include <sys/timerfd.h>

#include <string.h> // for bzero

using namespace mysdk;
using namespace mysdk::net;

namespace
{
	int createTimerfd()
	{
		int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
		if (timerfd < 0)
		{
			LOG_SYSFATAL << "Failed in timerfd_create";
		}
		return timerfd;
	}

	struct timespec howMuchTimeFromNow(Timestamp when)
	{
		int64 microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
		if (microseconds < 100)
		{
			microseconds = 100;
		}
		struct timespec ts;
		ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
		ts.tv_nsec = static_cast<long>( (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
		return ts;
	}

	void resetTimerfd(int timerfd, Timestamp when)
	{
		struct itimerspec newValue;
		struct itimerspec oldValue;
		bzero(&newValue, sizeof(newValue));
		bzero(&oldValue, sizeof(oldValue));
		newValue.it_value = howMuchTimeFromNow(when);
		int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
		if (ret)
		{
			LOG_SYSERR << "timerfd_settime()";
		}
	}
}

TimerQueue::TimerQueue(EventLoop* eventLoop):
		loop_(eventLoop),
		timerfd_(createTimerfd()),
		timerfdSession_(eventLoop, timerfd_),
		timers_()
{
	timerfdSession_.setReadCallback(std::tr1::bind(&TimerQueue::handleRead, this));
	timerfdSession_.enableReading();
}

TimerQueue::~TimerQueue()
{
	::close(timerfd_);

	for (TimerList::iterator it = timers_.begin(); it != timers_.end(); it++)
	{
		delete *it;
	}
	timers_.clear();
}

void TimerQueue::handleRead()
{
	Timestamp now(Timestamp::now());
	uint64 howmany;
	ssize_t n = ::read(timerfd_, &howmany, sizeof(howmany));
	//LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
	if (n != sizeof(howmany))
	{
		LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
	}

	TimerList expired;

	{
		MutexLockGuard lock(mutex_);
		TimerList::iterator it = timers_.begin();
		while (it != timers_.end() && (*it)->expiration() <= now)
		{
			++it;
		}
	    assert(it == timers_.end() || (*it)->expiration() > now);
	    expired.splice(expired.begin(), timers_, timers_.begin(), it);
	}

	for (TimerList::iterator it = expired.begin(); it != expired.end(); ++it)
	{
		(*it)->run();
	}

	Timestamp nextExpire;
	{
		MutexLockGuard lock(mutex_);
		for (TimerList::iterator it = expired.begin(); it != expired.end(); ++it)
		{
			if ( (*it)->repeat() )
			{
				(*it)->restart(now);
				insertWithLockHold(*it);
			}
			else
			{
				delete *it;
			}
		}
		if (!timers_.empty())
		{
			nextExpire = timers_.front()->expiration();
		}
	}

	if (nextExpire.valid())
	{
		resetTimerfd(timerfd_, nextExpire);
	}

}

TimerId TimerQueue::schedule(const TimerCallback& cb, Timestamp when, double interval)
{
	Timer* timer = new Timer(cb, when, interval);
	bool earliestChanged = false;
	{
	    MutexLockGuard lock(mutex_);
	    earliestChanged = insertWithLockHold(timer);
	}
	if (earliestChanged)
	{
		resetTimerfd(timerfd_, when);
	}

	return TimerId(timer);
}

bool TimerQueue::insertWithLockHold(Timer* timer)
{
	  bool earliestChanged = false;
	  Timestamp when = timer->expiration();
	  TimerList::iterator it = timers_.begin();
	  if (it == timers_.end() || (*it)->expiration() > when)
	  {
	    timers_.push_front(timer);
	    earliestChanged = true;
	  }
	  else
	  {
	    while (it != timers_.end() && (*it)->expiration() < when)
	    {
	      ++it;
	    }
	    timers_.insert(it, timer);
	  }
	  return earliestChanged;
}
