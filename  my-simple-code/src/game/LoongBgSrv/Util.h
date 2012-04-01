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

// return [nBegin, nEnd)
static inline int32 getRandomBetween(int32 nBegin, int32 nEnd)
{
	return nBegin + static_cast<int32>(static_cast<long long>(rand()) * (nEnd - nBegin) / RAND_MAX + 1);
	//return 0;
}

//获得加成数值
static inline int16 getBonusValue(BgUnit* me, BgUnit* target)
{
	static int16 bonusTable[4][4] = { {100, 100, 120, 100}, {120, 100, 100, 100}, {100, 120, 100, 100}, {0, 0, 0, 0}};

	int meUnitType = me->getUnitType() - 1;
	int targetUnitType = target->getUnitType() - 1;

	LOG_DEBUG << "getBonusValue - meId: " << me->getId()
							<< " meType: " << me->getUnitType()
							<< " target: " << target->getId()
							<< " targetType: " << target->getUnitType()
							<< " bonusTable: " << bonusTable[meUnitType][targetUnitType];
	return bonusTable[meUnitType][targetUnitType];
}

static inline uint32 getCurTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint32 seconds = tv.tv_sec;
	return seconds * 1000 + tv.tv_usec / 1000;
}

static inline uint16 GetHeight16(const int32 t)
{
	return static_cast<uint16>((t >> 16) & 0x0000FFFF);
}

static inline uint16 GetLow16(const int32 t)
{
	return static_cast<uint16>(t & 0x0000FFFF);
}

static inline int32 MakeInt32(const int16 height, const int16 low)
{
	return ( (height << 16) & 0x0000FFFF ) & (low & 0x0000FFFF);
}

#define TIME_FUNCTION_CALL(p, t) \
{       Timestamp oldTimeStamp = Timestamp::now(); \
        (p); \
        Timestamp newTimeStamp = Timestamp::now(); \
        double diff = timeDifference(newTimeStamp, oldTimeStamp); \
        if (diff > t)\
        LOG_WARN << "call function: " << #p << " , time[us] = " << diff; \
}


#endif /* UTIL_H_ */
