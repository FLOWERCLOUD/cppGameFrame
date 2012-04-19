/*
 * BgClientData.cc
 *
 *  Created on: 2012-4-19
 *    
 */

#include <game/LoongBgSrv/tests/BgClientData.h>
#include <mysdk/base/Logging.h>

BgClientData::BgClientData():
	playerId(0),
	name(""),
	petId(0),
	maxhp(0),
	hp(0),
	team(0),
	x(0),
	y(0)
{

}

bool BgClientData::isDead()
{
	return hp <= 0;
}

void BgClientData::printInfo()
{
	LOG_TRACE << " BgClientData::printInfo --- playerId: " << playerId
						<< " name: " << name
						<< " petId: " << petId
						<< " hp: " << hp
						<< " team: " << team
						<< " x: " << x
						<< " y: " << y;
}
