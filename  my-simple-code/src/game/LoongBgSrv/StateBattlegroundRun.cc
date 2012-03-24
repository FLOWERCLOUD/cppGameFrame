/*
 * StateBattlegroundRun.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundRun.h>
#include <game/LoongBgSrv/Battleground.h>
StateBattlegroundRun::StateBattlegroundRun(Battleground* bg):
	BattlegroundState(bg)
{

}

StateBattlegroundRun::~StateBattlegroundRun()
{

}

void StateBattlegroundRun::start()
{
	BattlegroundState::start();
	// 增加参加战场玩家的参战次数
	if (!pBattleground_) return;

	pBattleground_->incBgPlayerTimes();
}

void StateBattlegroundRun::run(uint32 curTime)
{
	if (!pBattleground_) return;

	if (curTime - startTime_ >= getStateTimeLimit())
	{
		pBattleground_->switchExitState();
	}
	else if (pBattleground_->haveOtherTeamEmpty())
	{
		pBattleground_->switchStartState();
	}
	else if (pBattleground_->isGameOver())
	{
		pBattleground_->switchExitState();
	}
}

void StateBattlegroundRun::end()
{
	if (!pBattleground_) return;

	if (!pBattleground_->haveOtherTeamEmpty())
	{
		pBattleground_->settlement();
	}
}

