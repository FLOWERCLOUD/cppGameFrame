
#ifndef MYSDK_NET_TIMERQUEUE_H
#define MYSDK_NET_TIMERQUEUE_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Mutex.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/Callbacks.h>
#include <mysdk/net/Session.h>

#include <list>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Timer;
	class TimerId;
	class TimerQueue
	{
	public:
		TimerQueue(EventLoop* loop);
		~TimerQueue();

		TimerId schedule(const TimerCallback& cb, Timestamp when, double interval);

	private:
		void handleRead();
		bool insertWithLockHold(Timer* timer);

		typedef std::list<Timer*> TimerList;

		EventLoop* loop_;
		const int timerfd_;
		Session timerfdSession_;
		MutexLock mutex_;
		TimerList timers_;
	private:
		DISALLOW_COPY_AND_ASSIGN(TimerQueue);
	};
}
}

#endif
