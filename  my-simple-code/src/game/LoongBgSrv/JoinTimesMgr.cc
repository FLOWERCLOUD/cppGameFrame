/*
 * JoinTimesMgr.cc
 *
 *  Created on: 2012-4-1
 *    
 */

#include <game/LoongBgSrv/JoinTimesMgr.h>

#include <mysdk/base/Logging.h>

JoinTimesMgr::JoinTimesMgr()
{
	init();
}

JoinTimesMgr::~JoinTimesMgr()
{

}


bool JoinTimesMgr::init()
{
	return true;
}

void JoinTimesMgr::run(uint32 curTime)
{

}

void JoinTimesMgr::shutdown()
{

}

void JoinTimesMgr::incJoinTimes(int32 playerId)
{
	PlayerMgrT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
		int32 joinTimes =  iter->second;
		playerMap_[playerId] = joinTimes++;
		LOG_INFO << "JoinTimesMgr::incJoinTimes - playerId: " << playerId
							<< " joinTimes: " << joinTimes;
		return;
	}

	LOG_INFO << "JoinTimesMgr::incJoinTimes - playerId: " << playerId
						<< " joinTimes: " << 0;
	playerMap_.insert(std::pair<int32, int32>(playerId, 0));
}

int32 JoinTimesMgr::getJoinTimes(int32 playerId)
{
	PlayerMgrT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
		int32 joinTimes =  iter->second;
		return joinTimes;
	}
	return 0;
}
