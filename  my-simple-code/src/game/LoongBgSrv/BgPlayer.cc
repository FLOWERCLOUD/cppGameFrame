/*
 * BgPlayer.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/BgPlayer.h>

#include <game/LoongBgSrv/base/PetBaseMgr.h>
#include <game/LoongBgSrv/base/SkillBaseMgr.h>
#include <game/LoongBgSrv/pet/Pet.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/skill/Buf.h>
#include <game/LoongBgSrv/skill/SkillHandler.h>
#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/BattlegroundMgr.h>
#include <game/LoongBgSrv/Util.h>

#include <mysdk/net/TcpConnection.h>

BgPlayer::BgPlayer(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	BgUnit(playerId, KNONE_UNITTYPE),
	name_(playerName),
	killEnemyTimes_(0),
	petId_(0),
	battlegroundId_(0),
	roleType_(0),
	title_(0),
	pScene(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
	LOG_DEBUG << "+++++BgPlayer::BgPlayer - playerId: " << playerId << name_;
}

BgPlayer::BgPlayer(int32 playerId, char* playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	BgUnit(playerId, KNONE_UNITTYPE),
	name_(playerName),
	killEnemyTimes_(0),
	petId_(0),
	battlegroundId_(0),
	roleType_(0),
	title_(0),
	pScene(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
	LOG_DEBUG << "+++++BgPlayer::BgPlayer - playerId: " << playerId << name_;
}

BgPlayer::~BgPlayer()
{
	LOG_DEBUG << "---------BgPlayer::~BgPlayer - playerId: " << this->getId() << name_;
}

void BgPlayer::setScene(Scene* scene)
{
	pScene = scene;
}

void BgPlayer::setBgId(int16 bgId)
{
	battlegroundId_ = bgId;
}

void BgPlayer::setRoleType(int32 roleType)
{
	roleType_ = roleType;
}

void BgPlayer::broadMsg(PacketBase& op)
{
	if (!pScene)
	{
		pScene->broadMsg(op);
	}
}

void BgPlayer::close()
{
	if (pScene)
	{
		pScene->removePlayer(this);
	}
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
	op.putInt32(unittId_);
	op.putInt32(unitType_);
	op.putInt32(team_);
	op.putUTF(name_);
	op.putInt32(roleType_);
	op.putInt32(petId_);
	op.putInt32(hp_);
	op.putInt32(title_);
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
	//  告诉客户端 xx 人中了buf
	PacketBase pb(client::OP_ADD_BUF, this->getId());
	pb.putInt32(buf->getId());
	broadMsg(pb);
	return true;
}

bool BgPlayer::hasSkill(int16 skillId)
{
	return true;
}

bool BgPlayer::canUseSkill(int16 skillId, int32 cooldownTime)
{
	std::map<int16, int32>::iterator iter;
	iter = useSkillMap_.find(skillId);
	if (iter != useSkillMap_.end())
	{
		int32 lastUseTime = iter->second;
		int32 curTime = getCurTime();
		if (curTime - lastUseTime <= cooldownTime)
		{
			return false;
		}
	}
	return true;
}

bool BgPlayer::useSkill(int16 skillId)
{
	int32 curTime = getCurTime();
	useSkillMap_.insert(std::pair<int16, int32>(skillId, curTime));
	return true;
}

void BgPlayer::onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill)
{
	BgUnit::onHurt(attacker, damage, skill);
	if (hp_ < damage)
	{
		damage = hp_;
	}
	decHp(damage);
	// 告诉客户端 你受到什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(0); //技能伤害
	pb.putInt32(skill.skillId_);
	pb.putInt32(skill.type_);
	pb.putInt32(damage);
	broadMsg(pb);
}

void BgPlayer::onBufHurt(BgUnit* me, int32 damage, const BufBase& buf)
{
	BgUnit::onBufHurt(me, damage, buf);
	if (hp_ < damage)
	{
		damage = hp_;
	}
	decHp(damage);
	// 告诉客户端 你受到什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(1); // buf 伤害
	pb.putInt32(buf.id_);
	pb.putInt32(damage);
	broadMsg(pb);
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
			// 告诉客户端 要移除这个buf
			PacketBase pb(client::OP_REMOVE_BUF, this->getId());
			pb.putInt32(buf->getId());
			broadMsg(pb);

			delete buf;
		}
		else
		{
			itList++;
		}
	}
}

#define BENGIN_MESSAGEHANDLE() switch (op) { \

#define INSERT_MESSAGEHANDLE(op, handle) case op: \
																							handle(pb); \
																							break; \

#define END_MESSAGEHANDLE() }



bool BgPlayer::onMsgHandler(PacketBase& pb)
{
	uint32 op = pb.getOP();
#if 1
	char hexop[10];
	snprintf(hexop, sizeof(hexop), "0x%X", op);
	LOG_DEBUG << "BgPlayer::onMsgHandler - playerId:"  << this->getId()
							<< " op: " << hexop;
	switch(op)
	{
	case game::OP_ENTER_BATTLE:
		onEnterBattle(pb);
		break;
	case game::OP_MOVE:
		onMove(pb);
		break;
	case game::OP_CHAT:
		onChat(pb);
		break;
	case game::OP_REQ_BATTLE_INFO:
		onReqBattleInfo(pb);
		break;
	case game::OP_STAND:
		onStand(pb);
		break;
	case game::OP_REQ_PLAYER_LIST:
		onReqPlayerList(pb);
		break;
	case game::OP_SELCET_PET:
		onSelectPet(pb);
		break;
	case game::OP_USE_SKILL:
		onUseSkill(pb);
		break;
	case game::OP_EXIT_BATTLE:
		onExitBattle(pb);
	default:
		break;
	}
#else

	BENGIN_MESSAGEHANDLE()
	INSERT_MESSAGEHANDLE(game::OP_ENTER_BATTLE, onEnterBattle)
	END_MESSAGEHANDLE()
#endif

	return true;
}

// 消息处理函数
void BgPlayer::onEnterBattle(PacketBase& pb)
{
	int16 bgId = pb.getInt16();
	int16 teamValue = pb.getInt16();

	if (!sBattlegroundMgr.checkBattlegroundId(bgId))
	{
		return;
	}

	BgUnit::TeamE team = (teamValue == BgUnit::kBlack_TEAM) ? BgUnit::kBlack_TEAM :BgUnit::kWhite_TEAM;

	PacketBase op(client::OP_ENTER_BATTLE, -1);
	op.putInt16(bgId);
	op.putInt16(teamValue);
	Battleground& bg = sBattlegroundMgr.getBattleground(bgId);
	if(bg.addBgPlayer(this, team))
	{
		setBgId(bgId);
		op.setOP(0);
	}
	this->sendPacket(op);
}

void BgPlayer::onMove(PacketBase& pb)
{
	if (!pScene) return;

	// 广播给其他玩家
	pb.setParam(client::OP_MOVE);
	pScene->broadMsg(pb);
}

void BgPlayer::onChat(PacketBase& pb)
{
	if (!pScene) return;

	pb.setOP(client::OP_CHAT);
	pScene->broadMsg(pb);
}

void BgPlayer::onReqBattleInfo(PacketBase& pb)
{
	int16 bgId = this->battlegroundId_;
	Battleground& bg = sBattlegroundMgr.getBattleground(bgId);
	pb.setOP(client::OP_REQ_BATTLE_INFO);
	if(bg.getBgInfo(pb))
	{
		this->sendPacket(pb);
	}
}

void BgPlayer::onStand(PacketBase& pb)
{
	int16 x = pb.getInt16();
	int16 y = pb.getInt16();
	setX(x);
	setY(y);
}

void BgPlayer::onReqPlayerList(PacketBase& pb)
{
	if (!pScene) return;

	std::map<int32, BgPlayer*>& playerMgr = pScene->getPlayerMgr();
	std::map<int32, BgPlayer*>::iterator iter;
	pb.setParam(playerMgr.size());
	for(iter = playerMgr.begin(); iter != playerMgr.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			player->serialize(pb);
		}
	}
	this->sendPacket(pb);
}

void BgPlayer::onSelectPet(PacketBase& pb)
{
	int16 petId = static_cast<int16>(pb.getParam());
	LOG_DEBUG << "BgPlayer::onSelectPet petId - " << petId << " playerId: " << this->getId();
	if (!sPetBaseMgr.checkPetId(petId))
	{
		return;
	}

	PetBase petbase = sPetBaseMgr.getPetBaseInfo(petId);
	setPetId(petId);
	setHp(petbase.hp_);

	petbase.serialize(pb);
	pb.setOP(client::OP_SELCET_PET);
	this->sendPacket(pb);
}

void BgPlayer::onUseSkill(PacketBase& pb)
{
	if (!pScene) return;

	int16 skillId = static_cast<int16>(pb.getParam());
	int32 playerId = pb.getInt32();
	LOG_DEBUG << "BgPlayer::onUseSkill skillID - " << skillId << " playerId: " << this->getId();

	BgPlayer* target = pScene->getPlayer(playerId);
	if (!target) return;

	// 不能攻击同一个队伍的人
	if (target->getTeam() != this->getTeam()) return;

	SkillHandler::onEmitSkill(skillId, this, target, pScene);
}

void BgPlayer::onExitBattle(PacketBase& pb)
{
	if (!pScene) return;
	pScene->removePlayer(this);
	pb.setOP(client::OP_EXIT_BATTLE);
	this->sendPacket(pb);
}
