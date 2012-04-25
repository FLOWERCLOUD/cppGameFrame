/*
 * StateBattlegroundCountDown.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundCountDown.h>

#include <game/LoongBgSrv/Battleground.h>

StateBattlegroundCountDown::StateBattlegroundCountDown(Battleground* bg):
	BattlegroundState(bg)
{

}

StateBattlegroundCountDown::~StateBattlegroundCountDown()
{

}

void StateBattlegroundCountDown::run(int64 curTime)
{
	if (!pBattleground_) return;

	if (curTime - startTime_ >= getStateTimeLimit())
	{
		if (pBattleground_->haveOtherTeamEmpty())
		{
			pBattleground_->switchStartState();
		}
		else
		{
			pBattleground_->switchRunState();
		}
	}
}

