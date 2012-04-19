/*
 * BgClientDataMgr.cc
 *
 *  Created on: 2012-4-19
 *    
 */

#include <game/LoongBgSrv/tests/BgClientDataMgr.h>

#include <game/LoongBgSrv/tests/BgClientData.h>

BgClientDataMgr::BgClientDataMgr()
{

}

BgClientDataMgr::~BgClientDataMgr()
{
	std::map<int32, BgClientData*>::iterator iter;
	for(iter = bgClientDataMap_.begin(); iter != bgClientDataMap_.end(); iter++)
	{
		BgClientData* player = iter->second;
		if (player)
		{
			delete player;
		}
	}
}

void BgClientDataMgr::addPlayer(BgClientData* bgClient)
{
	assert(bgClient);
	bgClientDataMap_.insert(std::pair<int32, BgClientData*>(bgClient->playerId, bgClient));
}

void BgClientDataMgr::removePlayer(int32 playerId)
{
	bgClientDataMap_.erase(playerId);
}

BgClientData* BgClientDataMgr::getPlayer(int32 playerId)
{
	std::map<int32, BgClientData*>::iterator iter;
	iter = bgClientDataMap_.find(playerId);
	if (iter != bgClientDataMap_.end())
	{
		return iter->second;
	}
	return NULL;
}

void BgClientDataMgr::printInfo()
{
	std::map<int32, BgClientData*>::iterator iter;
	for(iter = bgClientDataMap_.begin(); iter != bgClientDataMap_.end(); iter++)
	{
		BgClientData* player = iter->second;
		if (player)
		{
			player->printInfo();
		}
	}
}
