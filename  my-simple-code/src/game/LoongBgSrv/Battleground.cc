/*
 * Battleground.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/Battleground.h>

#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/BattlegroundState.h>
#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/StateBattlegroundCountDown.h>
#include <game/LoongBgSrv/StateBattlegroundExit.h>
#include <game/LoongBgSrv/StateBattlegroundRun.h>
#include <game/LoongBgSrv/StateBattlegroundStart.h>
#include <game/LoongBgSrv/StateBattlegroundWait.h>
#include <game/LoongBgSrv/JoinTimesMgr.h>
#include <game/LoongBgSrv/base/PetBase.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>

Battleground::Battleground():
	id_(0),
	bFirst_(true),
	bgResult_(KNONE_BGRESULT),
	pState_(NULL),
	blackBuildings_(1, KFOUR_UNITTYPE, BgUnit::kBlack_TEAM, &scene_),
	whiteBuildings_(2, KFOUR_UNITTYPE, BgUnit::kWhite_TEAM, &scene_)
{
	init();
}

Battleground::~Battleground()
{
	if (pState_)
	{
		delete pState_;
	}
}

void Battleground::setSrv(LoongBgSrv* srv)
{
	pSrv_ = srv;
}

void Battleground::init()
{
	bFirst_ = true;
	for (int i = 0; i < BgUnit::kCOUNT_TEAM; i++)
	{
		teamNum_[i] = 0;
	}
	teamNum_[0] = 99;

	blackBuildings_.init();
	whiteBuildings_.init();
	scene_.init();
}

void Battleground::shtudown()
{
	scene_.shutdown();
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
	LOG_DEBUG << " Battleground::addBgPlayer - playerId: " << player->getId()
							<< " team: " << team
							<< " num: " << static_cast<int>(teamNum_[team]);

	if (teamNum_[team] >= sMaxTeamNum)
	{
		//  告诉客户端, 这个队伍已经满人啦！不能进这个队伍啦
		return false;
	}

	TellPhpBattleInfo();

	if (bFirst_)
	{
		switchWaitState();
		bFirst_ = false;
	}
	teamNum_[team]++;
	player->setBgId(getId());
	player->setTeam(team);

	return scene_.addPlayer(player);
}

bool Battleground::removeBgPlayer(BgPlayer* player, BgUnit::TeamE team)
{
	LOG_DEBUG << " Battleground::removeBgPlayer - playerId: " << player->getId()
							<< " team: " << team
							<< " num: " << static_cast<int>(teamNum_[team]);

	TellPhpBattleInfo();

	teamNum_[team]--;
	player->setBgId(0);
	player->setTeam(BgUnit::kNONE_TEAM);
	return scene_.removePlayer(player);
}

void Battleground::run(uint32 curTime)
{
	scene_.run(curTime);
	if (pState_)
	{
		pState_->run(curTime);
	}
}

bool Battleground::getBgInfo(PacketBase& op)
{
	op.putInt32(teamNum_[BgUnit::kBlack_TEAM]);
	op.putInt32(teamNum_[BgUnit::kWhite_TEAM]);
	op.putInt32(getState());
	op.putInt32(getLeftTime());
	op.putInt32(blackBuildings_.getHp());
	op.putInt32(whiteBuildings_.getHp());
	scene_.serializeItem(op);
	//
	return true;
}

BattlegroundState::BgStateE Battleground::getState()
{
	if (pState_)
	{
		return pState_->getState();
	}
	return BattlegroundState::BGSTATE_WAIT;
}

uint32 Battleground::getLeftTime()
{
	if (pState_)
	{
		return pState_->getLeftTime();
	}
	return 999999;
}

BgUnit* Battleground::getTargetUnit(int32 playerId, int32 uintType)
{
	enum
	{
		PLAYER_UNITTYPE	= 	0,
		BLACK_BUILDING_UNITTYPE = 1,
		WHITE_BUILDING_UNITTYPE = 2,
	};

	BgUnit* target = NULL;
	if (uintType == PLAYER_UNITTYPE)
	{
		target = scene_.getPlayer(playerId);
	}
	else if (uintType == BLACK_BUILDING_UNITTYPE)
	{
		return &blackBuildings_;
	}
	else if (uintType == WHITE_BUILDING_UNITTYPE)
	{
		return &whiteBuildings_;
	}
	return target;
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

void Battleground::TellClientCloseBg(int32 state)
{
	PacketBase op(client::OP_MEMBER_INSUFFICIENT, state); // state 0 代表正常退出战场  1 代表敌方人数不足退出战场
	scene_.broadMsg(op);
}

bool Battleground::isGameOver()
{
	if (blackBuildings_.isDead() && whiteBuildings_.isDead())
	{
		bgResult_ = KDRAW_BGRESULT;
		return true;
	}

	if (blackBuildings_.isDead())
	{
		bgResult_ = KWHITE_BGRESULT;
		return true;
	}

	if (whiteBuildings_.isDead())
	{
		bgResult_ = KDRAW_BGRESULT;
		return true;
	}
	return false;
}

void Battleground::settlement()
{
	//统计战场结果 发放战斗奖励
	std::map<int32, BgPlayer*>& playerMgr = scene_.getPlayerMgr();
	std::map<int32, BgPlayer*>::iterator iter;

	PacketBase op(client::OP_SETTLEMENT, playerMgr.size());
	op.putInt32(bgResult_);
	PacketBase hotelop(hotel::OP_GET_BATTLEGROUND_AWARD, playerMgr.size());
	for(iter = playerMgr.begin(); iter != playerMgr.end(); iter++)
	{
			BgPlayer* player = iter->second;
			if (player)
			{
				hotelop.putInt32(id_);
				hotelop.putInt32(id_);
				player->serializeResult(op, bgResult_, hotelop);
			}
	}

	scene_.broadMsg(op);
	// 把战斗结果 通知给hotel 让hotel把相应的奖励加给玩家哦
	sendToHotel(hotelop);
}

void Battleground::incBgPlayerTimes()
{
	std::map<int32, BgPlayer*>& playerMgr = scene_.getPlayerMgr();
	std::map<int32, BgPlayer*>::iterator iter;
	for(iter = playerMgr.begin(); iter != playerMgr.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			sJoinTimesMgr.incJoinTimes(player->getId());
		}
	}
}

void Battleground::tellClientBgState()
{
	PacketBase op(client::OP_TELLCLIENT_STATE, 0);
	op.putInt32(getState());
	op.putInt32(getLeftTime());
	scene_.broadMsg(op);

	TellPhpBattleInfo();
}

bool Battleground::haveOtherTeamEmpty()
{
	if (teamNum_[BgUnit::kBlack_TEAM] == sMinTeamNum || teamNum_[BgUnit::kWhite_TEAM] == sMinTeamNum)
	{
		return true;
	}
	return false;
}

void Battleground::TellPhpBattleInfo()
{
	if (pSrv_)
	{
		pSrv_->TellPhpBattleInfo(getId());
	}
}

void Battleground::sendToHotel(PacketBase& pb)
{
	if (pSrv_)
	{
		pSrv_->sendToHotel(pb);
	}
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
	LOG_DEBUG << "Battleground::closeBattleground - battlegroundId: " << this->getId();
	if (pState_)
	{
		pState_->end();
		// 释放掉 前面状态的内存
		delete pState_;
		pState_ = NULL;
	}

	std::map<int32, BgPlayer*>& playerMgr = scene_.getPlayerMgr();
	std::map<int32, BgPlayer*>::iterator iter;
	for(iter = playerMgr.begin(); iter != playerMgr.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			player->setBgId(0);
			player->setTeam(BgUnit::kNONE_TEAM);
		}
	}

	// 重新初始化一下战场哦
	init();

	TellPhpBattleInfo();
}
