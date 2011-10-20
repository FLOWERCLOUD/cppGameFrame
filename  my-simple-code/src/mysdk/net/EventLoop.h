
#ifndef MYSDK_NET_EVENTLOOP_H
#define MYSDK_NET_EVENTLOOP_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Callbacks.h>
#include <mysdk/net/TimerQueue.h>
#include <mysdk/net/TimerId.h>
#include <mysdk/net/Session.h>

#include <tr1/functional>
#include <vector>

namespace mysdk
{
namespace net
{
	class Poller;
	class EventLoop
	{
	public:
		typedef std::tr1::function<void()> Functor;
	public:
		EventLoop();
		~EventLoop();

		void loop();
		void quit();

		Timestamp pollReturnTime() const { return pollReturnTime_; }

		TimerId runAt(const Timestamp& time, const TimerCallback& cb);
		TimerId runAfter(double delay, const TimerCallback& cb);
		TimerId runEvery(double interval, const TimerCallback& cb);

		void queueInLoop(const Functor& cb);

		void wakeup();
		void addEvent(Session* session, const int mask);
		void delEvent(Session* session, const int delmask);
	private:
		void handleRead();

		void doPendingFunctors();

		void printActiveSessions() const;

		typedef std::vector<Session*> SessionList;

		bool looping_;
		bool quit_;
		bool eventHanding_;
		bool callingPendingFunctors_;

		//int wakeupFd_;
		//Session wakeupSession_;
		Timestamp pollReturnTime_;

		int wakeupFd_;
		Session* pWakeupSession_;
		Poller* pPoller_;
		TimerQueue* pTimerQueue_;

		SessionList activeSessions_;

		std::vector<Functor> pendingFunctors_;
	private:
		DISALLOW_COPY_AND_ASSIGN(EventLoop);
	};
}
}

#endif
