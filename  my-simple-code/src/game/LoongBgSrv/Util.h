/*
 * Util.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_UTIL_H_
#define GAME_UTIL_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BgUnit.h>

#include <stdlib.h>

using namespace mysdk;

static inline int16 getDistance(BgUnit* me, BgUnit* target)
{
	int16 deltaX = static_cast<int16>(me->getX() - target->getX());
	int16 deltaY = static_cast<int16>(me->getY() - target->getY());
	return static_cast<int16>(deltaX * deltaX + deltaY * deltaY);
}

static inline int32 getRandomBetween(int32 nBegin, int32 nEnd)
{
	return nBegin + static_cast<int32>(static_cast<long long>(rand()) * (nEnd - nBegin) / RAND_MAX + 1);
	return 0;
}

//获得加成数值
static inline int16 getBonusValue(BgUnit* me, BgUnit* target)
{
	static int16 bonusTable[4][4] = { {100, 100, 120, 100}, {120, 100, 100, 100}, {100, 120, 100, 100}, {0, 0, 0, 0}};

	int meUnitType = me->getUnitType() - 1;
	int targetUnitType = target->getUnitType() - 1;
	return bonusTable[meUnitType][targetUnitType];
}

static inline uint32 getCurTime()
{
	return 0;
}

static inline uint16 GetHeight16(const int32 t)
{
	return static_cast<uint16>((t >> 16) & 0x0000FFFF);
}

static inline uint16 GetLow16(const int32 t)
{
	return static_cast<uint16>(t & 0x0000FFFF);
}

#endif /* UTIL_H_ */
