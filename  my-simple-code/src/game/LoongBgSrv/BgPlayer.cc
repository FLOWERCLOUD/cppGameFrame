/*
 * BgPlayer.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/BgPlayer.h>

#include <game/LoongBgSrv/pet/Pet.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/skill/Buf.h>
#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/BattlegroundMgr.h>

#include <mysdk/net/TcpConnection.h>

BgPlayer::BgPlayer(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	BgUnit(playerId, KNONE_UNITTYPE),
	name_(playerName),
	killEnemyTimes_(0),
	petId_(0),
	pScene(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
}

BgPlayer::BgPlayer(int32 playerId, char* playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	BgUnit(playerId, KNONE_UNITTYPE),
	name_(playerName),
	killEnemyTimes_(0),
	petId_(0),
	pScene(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
}

BgPlayer::~BgPlayer()
{

}

void BgPlayer::setScene(Scene* scene)
{
	pScene = scene;
}

void BgPlayer::incKillEnemyTime()
{
	killEnemyTimes_++;
}

bool BgPlayer::hasPet()
{
	return petId_ > 0;
}

void BgPlayer::setPetId(int16 petId)
{
	petId_ = petId;
}

void BgPlayer::sendPacket(PacketBase& op)
{
	if (pSrv_ && pCon_)
	{
		pSrv_->send(pCon_, op);
	}
}

void BgPlayer::run(uint32 curTime)
{
	runBuf(curTime);
}

bool BgPlayer::serialize(PacketBase& op)
{
	return true;
}

bool BgPlayer::canSkillHurt()
{
	return true;
}

bool BgPlayer::canBufHurt()
{
	return true;
}

bool BgPlayer::addBuf(Buf* buf)
{
	bufList_.push_back(buf);
	return true;
}

bool BgPlayer::hasSkill(int16 skillId)
{
	return true;
}

bool BgPlayer::canUseSkill(int16 skillId)
{
	return true;
}

void BgPlayer::onHurt(BgUnit* attacker, int32 damage, int16 skillId)
{
	BgUnit::onHurt(attacker, damage, skillId);
	decHp(damage);
}

void BgPlayer::runBuf(uint32 curTime)
{
	std::list<Buf*>::iterator itList;
	for (itList = bufList_.begin(); itList != bufList_.end(); )
	{
		Buf* buf = *itList;
		assert(buf);
		buf->run(this, curTime);
		if (buf->waitDel())
		{
			itList = bufList_.erase(itList);
		}
		else
		{
			itList++;
		}
	}
}

bool BgPlayer::onMsgHandler(PacketBase& pb)
{
	uint32 op = pb.getOP();
	switch(op)
	{
	case game::ENTER_BATTLE:
		onEnterBattle(pb);
		break;
	case game::MOVE:
		onMove(pb);
		break;
	case game::CHAT:
		onChat(pb);
		break;
	default:
		break;
	}
	return true;
}

void BgPlayer::onEnterBattle(PacketBase& pb)
{
	int16 bgId = pb.getInt16();
	int16 teamValue = pb.getInt16();

	if (!sBattlegroundMgr.checkBattlegroundId(bgId))
	{
		return;
	}

	BgUnit::TeamE team = (teamValue == BgUnit::kBlack_TEAM) ? BgUnit::kBlack_TEAM :BgUnit::kWhite_TEAM;

	Battleground& bg = sBattlegroundMgr.getBattleground(bgId);
	bg.addBgPlayer(this, team);
}

void BgPlayer::onMove(PacketBase& pb)
{
	if (!pScene) return;

	int16 x = pb.getInt16();
	int16 y = pb.getInt16();
	setX(x);
	setY(y);

	// 广播给其他玩家
	PacketBase op(client::MOVE, this->getId());
	op.putInt16(x);
	op.putInt16(y);
	pScene->broadMsg(op);
}

void BgPlayer::onChat(PacketBase& pb)
{
	if (!pScene) return;

	pb.setOP(client::CHAT);
	pScene->broadMsg(pb);
}
