/*
 * BattlegroundState.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/BattlegroundState.h>

#include <game/LoongBgSrv/Battleground.h>
#include <game/LoongBgSrv/Util.h>

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
	startTime_ = getCurTime();
	if (pBattleground_)
	{
		pBattleground_->tellClientBgState();
	}
}

void BattlegroundState::run(uint32 curTime)
{

}

void BattlegroundState::end()
{

}

uint32 BattlegroundState::getLeftTime()
{
	uint32 curTime = getCurTime();
	uint32 leftTime = getStateTimeLimit() - (curTime - startTime_);
	return leftTime;
}
