/*
 * StateBattlegroundRun.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/StateBattlegroundRun.h>
#include <game/LoongBgSrv/Battleground.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>

StateBattlegroundRun::StateBattlegroundRun(Battleground* bg):
	BattlegroundState(bg),
	bWaitState_(false),
	waitTimes_(0)
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

	if (bWaitState_)
	{
		if (!pBattleground_->haveOtherTeamEmpty())
		{
			PacketBase op(client::OP_TELLCLIENT_STATE, 0);
			op.putInt32(getState());
			op.putInt32(getLeftTime());
			pBattleground_->broadMsg(op);
			bWaitState_ = false;
		}
		else if (curTime - waitTimes_ > 10000)
		{
			pBattleground_->switchExitState();
		}
		return;
	}

	if (curTime - startTime_ >= getStateTimeLimit())
	{
		pBattleground_->switchExitState();
	}
	else if (pBattleground_->haveOtherTeamEmpty())
	{
		PacketBase op(client::OP_TELLCLIENT_STATE, 0);
		op.putInt32(3);
		op.putInt32(getLeftTime());
		pBattleground_->broadMsg(op);

		bWaitState_ = true;
		waitTimes_ = curTime;
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

