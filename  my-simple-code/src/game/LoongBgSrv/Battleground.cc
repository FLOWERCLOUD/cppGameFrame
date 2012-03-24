/*
 * Battleground.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/Battleground.h>

#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/BattlegroundState.h>
#include <game/LoongBgSrv/StateBattlegroundCountDown.h>
#include <game/LoongBgSrv/StateBattlegroundExit.h>
#include <game/LoongBgSrv/StateBattlegroundRun.h>
#include <game/LoongBgSrv/StateBattlegroundStart.h>
#include <game/LoongBgSrv/StateBattlegroundWait.h>

Battleground::Battleground():
	id_(0),
	bFirst_(true),
	pState_(NULL)
{
	init();
}

Battleground::~Battleground()
{
}

void Battleground::init()
{
	bFirst_ = true;
	for (int i = 0; i < BgUnit::kCOUNT_TEAM; i++)
	{
		teamNum_[i] = 0;
	}
	teamNum_[0] = 99;
}

void Battleground::setId(int32 Id)
{
	id_ = Id;
}

int32 Battleground::getId()
{
	return id_;
}

bool Battleground::addBgPlayer(BgPlayer* player, BgUnit::TeamE team)
{
	if (teamNum_[team] >= sMaxTeamNum)
	{
		//  告诉客户端, 这个队伍已经满人啦！不能进这个队伍啦
		return false;
	}

	if (bFirst_)
	{
		switchWaitState();
		bFirst_ = false;
	}
	teamNum_[team]++;
	player->setTeam(team);
	return scene_.addPlayer(player);
}

bool Battleground::removeBgPlayer(BgPlayer* player, BgUnit::TeamE team)
{
	teamNum_[team]--;
	player->setTeam(BgUnit::kNONE_TEAM);
	return scene_.removePlayer(player);
}

void Battleground::run(uint32 curTime)
{
	scene_.run(curTime);
}

bool Battleground::isFull()
{
	if (teamNum_[BgUnit::kBlack_TEAM] >= sMaxTeamNum && teamNum_[BgUnit::kWhite_TEAM]  >= sMaxTeamNum)
	{
		return true;
	}
	return false;
}

bool Battleground::isEmpty()
{
	if (teamNum_[BgUnit::kBlack_TEAM] <= sMinTeamNum && teamNum_[BgUnit::kWhite_TEAM] <= sMinTeamNum)
	{
		return true;
	}
	return false;
}

bool Battleground::isGameOver()
{
	return false;
}

void Battleground::settlement()
{

}

void Battleground::incBgPlayerTimes()
{

}

void Battleground::tellClientBgState()
{

}

bool Battleground::haveOtherTeamEmpty()
{
	if (teamNum_[BgUnit::kBlack_TEAM] == sMinTeamNum || teamNum_[BgUnit::kWhite_TEAM] == sMinTeamNum)
	{
		return true;
	}
	return false;
}

void Battleground::setBattlegroundState(BattlegroundState* state)
{
	if (pState_)
	{
		pState_->end();
		// 释放掉 前面状态的内存
		delete pState_;
		pState_ = NULL;
	}
	pState_ = state;
	if (pState_)
	{
		pState_->start();
	}
}

void Battleground::switchWaitState()
{
	BattlegroundState* state = new StateBattlegroundWait(this);
	setBattlegroundState(state);
}

void Battleground::switchCountDownState()
{
	BattlegroundState* state = new StateBattlegroundCountDown(this);
	setBattlegroundState(state);
}

void Battleground::switchStartState()
{
	BattlegroundState* state = new StateBattlegroundStart(this);
	setBattlegroundState(state);
}

void Battleground::switchRunState()
{
	BattlegroundState* state = new StateBattlegroundRun(this);
	setBattlegroundState(state);
}

void Battleground::switchExitState()
{
	BattlegroundState* state = new StateBattlegroundExit(this);
	setBattlegroundState(state);
}

void Battleground::closeBattleground()
{
	if (pState_)
	{
		pState_->end();
		// 释放掉 前面状态的内存
		delete pState_;
		pState_ = NULL;
	}
	// 重新初始化一下战场哦
	init();
}
