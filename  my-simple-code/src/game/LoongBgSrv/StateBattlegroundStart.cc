/*
 * StateBattlegroundStart.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundStart.h>
#include <game/LoongBgSrv/Battleground.h>
StateBattlegroundStart::StateBattlegroundStart(Battleground* bg):
	BattlegroundState(bg)
{

}

StateBattlegroundStart::~StateBattlegroundStart()
{

}

void StateBattlegroundStart::run(uint32 curTime)
{
	if (!pBattleground_) return;

	if (curTime - startTime_ >= getStateTimeLimit())
	{
		pBattleground_->TellClientCloseBg(1);
		pBattleground_->switchExitState();
	}
	else if (! pBattleground_->haveOtherTeamEmpty())
	{
		pBattleground_->switchRunState();
	}
}
