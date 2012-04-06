/*
 * BgPlayerMgr.cc
 *
 *  Created on: 2012-4-1
 *    
 */

#include <game/LoongBgSrv/BgPlayerMgr.h>


BgPlayerMgr::BgPlayerMgr()
{

}

BgPlayerMgr::~BgPlayerMgr()
{

}

bool BgPlayerMgr::hasBgPlayer(int32 playerId)
{
	BgPlayerMapT::iterator iter;
	iter = bgPlayerMap_.find(playerId);
	if (iter != bgPlayerMap_.end())
	{
		return true;
	}
	return false;
}

void BgPlayerMgr::addBgPlayer(int32 playerId, BgPlayer* player)
{
	bgPlayerMap_.insert(std::pair<int32, BgPlayer*>(playerId, player));
}

void BgPlayerMgr::removeBgPlayer(int32 playerId)
{
	bgPlayerMap_.erase(playerId);
}

