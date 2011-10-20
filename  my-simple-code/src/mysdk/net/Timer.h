
#ifndef MYSDK_NET_TIMER_H
#define MYSDK_NET_TIMER_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/Callbacks.h>

namespace mysdk
{
namespace net
{
	class Timer
	{
	public:
		Timer(const TimerCallback& cb, Timestamp when, double interval):
			callback_(cb),
			expiration_(when),
			interval_(interval),
			repeat_(interval > 0.0)
		{}

		void run() const
		{
			callback_();
		}

		Timestamp expiration() const { return expiration_; }
		bool repeat() const { return repeat_; }

		void restart(Timestamp now);
	private:
		const TimerCallback callback_;
		Timestamp expiration_;
		const double interval_;
		const bool repeat_;
	private:
		DISALLOW_COPY_AND_ASSIGN(Timer);
	};
}
}

#endif
