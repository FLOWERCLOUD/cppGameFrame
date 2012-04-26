/*
 * JoinTimesMgr.cc
 *
 *  Created on: 2012-4-1
 *    
 */

#include <game/LoongBgSrv/JoinTimesMgr.h>

#include <game/LoongBgSrv/Util.h>
#include <mysdk/base/Logging.h>

JoinTimesMgr::JoinTimesMgr():
	lastTimes_(getCurTime())
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
#ifndef TEST
	int64 curTime = getCurTime();
	if (!isSameDay(lastTimes_, curTime))
	{
		LOG_INFO << "JoinTimesMgr::incJoinTimes - second day!!! lastTimes_ " << lastTimes_
						<< "  curTime: " << curTime;

		lastTimes_ = curTime;
		playerMap_.clear();
	}

	PlayerMgrT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
			int32 joinTimes =  iter->second;
			playerMap_[playerId] = ++joinTimes;
			LOG_INFO << "JoinTimesMgr::incJoinTimes - playerId: " << playerId
							<< " joinTimes: " << joinTimes;
			return;
	}

	LOG_INFO << "JoinTimesMgr::incJoinTimes - playerId: " << playerId
						<< " joinTimes: " << 1;
	playerMap_.insert(std::pair<int32, int32>(playerId, 1));
#endif
}

int32 JoinTimesMgr::getJoinTimes(int32 playerId)
{
#ifndef TEST
	PlayerMgrT::iterator iter;
	iter = playerMap_.find(playerId);
	if (iter != playerMap_.end())
	{
		int32 joinTimes =  iter->second;
		return joinTimes;
	}
#endif
	return 0;
}
