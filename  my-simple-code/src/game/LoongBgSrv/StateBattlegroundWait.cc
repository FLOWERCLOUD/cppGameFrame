/*
 * StateBattlegroundWait.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundWait.h>
#include <game/LoongBgSrv/Battleground.h>
StateBattlegroundWait::StateBattlegroundWait(Battleground* bg):
	BattlegroundState(bg)
{

}

StateBattlegroundWait::~StateBattlegroundWait()
{

}

void StateBattlegroundWait::run(int64 curTime)
{
	if (!pBattleground_) return;

	if (pBattleground_->isFull())
	{
		pBattleground_->switchCountDownState();
	}
	else if (pBattleground_->isEmpty())
	{
		pBattleground_->switchExitState();
	}
	else if (curTime - startTime_ >= getStateTimeLimit())
	{
		pBattleground_->switchCountDownState();
	}
}

