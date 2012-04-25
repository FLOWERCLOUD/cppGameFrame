/*
 * StateBattlegroundExit.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundExit.h>
#include <game/LoongBgSrv/Battleground.h>
StateBattlegroundExit::StateBattlegroundExit(Battleground* bg):
	BattlegroundState(bg)
{
}

StateBattlegroundExit::~StateBattlegroundExit()
{

}

void StateBattlegroundExit::run(int64 curTime)
{
	if (!pBattleground_) return;

	if (curTime - startTime_ >= getStateTimeLimit())
	{
		pBattleground_->TellClientCloseBg(0);
		pBattleground_->closeBattleground();
	}
}
