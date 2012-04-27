/*
 * Util.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_UTIL_H_
#define GAME_UTIL_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Logging.h>

#include <game/LoongBgSrv/BgUnit.h>

#include <sys/time.h>
#include <stdlib.h>

using namespace mysdk;

static inline int32 getDistance(BgUnit* me, BgUnit* target)
{
	int32 deltaX = (me->getX() - target->getX());
	int32 deltaY = (me->getY() - target->getY());
	return (deltaX * deltaX + deltaY * deltaY);
}

// return [nBegin, nEnd]
static inline int32 getRandomBetween(int32 nBegin, int32 nEnd)
{
	return nBegin + static_cast<int32>(random() % (nEnd - nBegin + 1) );
	//return nBegin + static_cast<int32>(static_cast<long long>(rand()) * (nEnd - nBegin) / (RAND_MAX + 1) );
	//return 0;
}

//获得加成数值
static inline int16 getBonusValue(BgUnit* me, BgUnit* target)
{
	static int16 bonusTable[5][5] = { {100, 100, 150, 100, 100}, {150, 100, 100, 100, 100}, {100, 150, 100, 100, 100}, {100, 100, 100, 100, 100},  {100, 100, 100, 100, 100}};

	int meUnitType = me->getUnitType() - 1;
	int targetUnitType = target->getUnitType() - 1;

	LOG_DEBUG << "getBonusValue - meId: " << me->getId()
							<< " meType: " << me->getUnitType()
							<< " target: " << target->getId()
							<< " targetType: " << target->getUnitType()
							<< " bonusTable: " << bonusTable[meUnitType][targetUnitType];
	return bonusTable[meUnitType][targetUnitType];
}

static inline int64 getCurTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64 seconds = tv.tv_sec;
	return seconds * 1000 + tv.tv_usec / 1000;
}

// 这个实现很那个... 谁能提供一个比较好的实现呢？
static inline bool isSameDay(int64 day1, int64 day2)
{
    static const int kMilliSecondsPerDay = 24 * 60 * 60 * 1000;
    static const int kUTFDifference = 8 * 60 * 60 * 1000; //utf 时间和北京时间的差（差8小时）
    return (day1 + kUTFDifference)  / kMilliSecondsPerDay == (day2 + kUTFDifference) / kMilliSecondsPerDay;
}

static inline uint16 GetHeight16(const int32 t)
{
	return static_cast<uint16>((t >> 16) & 0x0000FFFF);
}

static inline uint16 GetLow16(const int32 t)
{
	return static_cast<uint16>(t & 0x0000FFFF);
}

static inline int32 MakeInt32(uint16 height, uint16 low)
{
	return ( ((height << 16) & 0xFFFF0000 ) | (low & 0x0000FFFF));
}

#define TIME_FUNCTION_CALL(p, t) \
{       Timestamp oldTimeStamp = Timestamp::now(); \
        (p); \
        Timestamp newTimeStamp = Timestamp::now(); \
        double diff = timeDifference(newTimeStamp, oldTimeStamp); \
        if (diff > t)\
        LOG_WARN << "[YOUHUA] call function: " << #p << " , time[s] = " << diff; \
}

static inline void Log(const char* content)
{
	char logbuf[256] = {0};
	time_t seconds = static_cast<time_t>(Timestamp::now().microSecondsSinceEpoch() / Timestamp::kMicroSecondsPerSecond);
	struct tm tm_time;
	localtime_r(&seconds, &tm_time);

	snprintf(logbuf, sizeof(logbuf) - 1, "[%4d-%02d-%02d-%02d-%02d-%02d][LOG] [DB]%s",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			content);

	printf("%s\n", logbuf);
}

#endif /* UTIL_H_ */
