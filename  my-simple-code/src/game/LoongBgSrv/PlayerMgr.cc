/*
 * PlayerMgr.cc
 *
 *  Created on: 2012-3-31
 *    
 */

#include <game/LoongBgSrv/PlayerMgr.h>

#include <game/LoongBgSrv/Player.h>

PlayerMgr::PlayerMgr()
{

}

PlayerMgr::~PlayerMgr()
{

}


bool PlayerMgr::isOnline(int32 playerId)
{
	PlayerMgrMapT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
		Player* player =  iter->second;
		if (player)
		{
			return player->isOnline();
		}
	}

	return false;
}

bool PlayerMgr::addPlayer(int32 playerId, Player* player)
{
	playerMap_.insert(std::pair<int32, Player*>(playerId, player));
	return true;
}

Player* PlayerMgr::getPlayer(int32 playerId)
{
	PlayerMgrMapT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
		return iter->second;
	}
	return NULL;
}
