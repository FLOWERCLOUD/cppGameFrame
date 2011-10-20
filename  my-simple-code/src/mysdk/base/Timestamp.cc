#include <mysdk/base/Timestamp.h>

#include <sys/time.h>
#include <stdio.h>

using namespace mysdk;

Timestamp::Timestamp():
		microSecondsSinceEpoch_(0)
{

}

Timestamp::Timestamp(int64 microseconds):
		microSecondsSinceEpoch_(microseconds)
{

}

string Timestamp::toString() const
{
	char buf[32] = {0};
	int64 seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
	int64 microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
	snprintf(buf, sizeof(buf) - 1, "%" MYSDK_LL_FORMAT "d.%06" MYSDK_LL_FORMAT "d", seconds, microseconds);
	return buf;
}

string Timestamp::toFormattedString() const
{
	char buf[32] = {0};
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);

	snprintf(buf, sizeof(buf) - 1, "%4d-%02d-%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			microseconds);

	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64 seconds = tv.tv_sec;
	return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
	return Timestamp();
}
