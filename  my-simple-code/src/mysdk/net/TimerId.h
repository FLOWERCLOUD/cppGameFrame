
#ifndef 	MYSDK_NET_TIMERID_H
#define	MYSDK_NET_TIMERID_H

#include <mysdk/base/Common.h>

namespace mysdk
{
namespace net
{
	class Timer;
	class TimerId
	{
	public:
		explicit TimerId(Timer* timer):
			value_(timer)
		{
		}
	private:
		Timer* value_;
	};
}
}

#endif
