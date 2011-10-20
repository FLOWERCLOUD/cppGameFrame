
#include <mysdk/net/Timer.h>

using namespace mysdk;
using namespace mysdk::net;

void Timer::restart(Timestamp now)
{
	if (repeat_)
	{
		expiration_ = addTime(now, interval_);
	}
	else
	{
		expiration_ = Timestamp::invalid();
	}
}

