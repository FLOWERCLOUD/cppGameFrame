/*
 * BattlegroundState.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/BattlegroundState.h>

#include <game/LoongBgSrv/Battleground.h>
#include <game/LoongBgSrv/Util.h>

#include <mysdk/base/Logging.h>

BattlegroundState::BattlegroundState(Battleground* bg):
	pBattleground_(bg),
	startTime_(0)
{

}

BattlegroundState::~BattlegroundState()
{

}

void BattlegroundState::start()
{
	LOG_DEBUG << "BattlegroundState::start " << getStateName();
	startTime_ = getCurTime();
	if (pBattleground_)
	{
		pBattleground_->tellClientBgState();
	}
}

void BattlegroundState::run(int64 curTime)
{

}

void BattlegroundState::end()
{
	LOG_DEBUG << "BattlegroundState::end " << getStateName();
}

uint32 BattlegroundState::getLeftTime()
{
	int64 curTime = getCurTime();
	uint32 leftTime = getStateTimeLimit() - static_cast<uint32>(curTime - startTime_);
	return leftTime;
}
