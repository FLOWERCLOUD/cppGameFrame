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
#include <game/LoongBgSrv/skill/ItemHandler.h>
#include <game/LoongBgSrv/skill/SkillHandler.h>
#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/BattlegroundMgr.h>
#include <game/LoongBgSrv/Util.h>

#include <mysdk/net/TcpConnection.h>

#define sBattlegroundMgr pSrv_->getBgMgr()

BgPlayer::BgPlayer(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	BgUnit(playerId, KNONE_UNITTYPE),
	name_(playerName),
	killEnemyTimes_(0),
	petId_(0),
	battlegroundId_(0),
	roleType_(0),
	title_(0),
	joinTimes_(0),
	bWaitClose_(false),
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
	joinTimes_(0),
	bWaitClose_(false),
	pScene(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
	LOG_DEBUG << "+++++BgPlayer::BgPlayer - playerId: " << playerId << name_;
}

BgPlayer::BgPlayer(int32 playerId, char* playerName, int32 roleType, int32 joinTimes, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
			BgUnit(playerId, KNONE_UNITTYPE),
			name_(playerName),
			killEnemyTimes_(0),
			petId_(0),
			battlegroundId_(0),
			roleType_(roleType),
			title_(0),
			joinTimes_(joinTimes),
			bWaitClose_(false),
			pScene(NULL),
			pCon_(pCon),
			pSrv_(pSrv)
{
	LOG_DEBUG << "+++++BgPlayer::BgPlayer - playerId: " << playerId
							<< " name: " <<name_;
}

BgPlayer::~BgPlayer()
{
	removeAllBuf();
	bufList_.clear();
	useSkillMap_.clear();
	LOG_DEBUG << "---------BgPlayer::~BgPlayer - playerId: " << this->getId()
							<< " name: " << name_;
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

void BgPlayer::setJoinTimes(int32 joinTimes)
{
	joinTimes_ = joinTimes;
}

void BgPlayer::broadMsg(PacketBase& op)
{
	if (pScene)
	{
		pScene->broadMsg(op);
	}
}

void BgPlayer::close()
{
	int16 bgId = battlegroundId_;
	if (sBattlegroundMgr.checkBattlegroundId(bgId))
	{
		Battleground& bg = sBattlegroundMgr.getBattleground(bgId);
		bg.removeBgPlayer(this, team_);
	}


	//if (pScene)
	//{
	//	pScene->removePlayer(this);
	//}
}

void BgPlayer::setWaitClose(bool flag)
{
	bWaitClose_ = true;
	close();
}

bool BgPlayer::getWaitClose()
{
	return bWaitClose_;
}

bool BgPlayer::addItem(int32 itemId)
{
	if (package_.addItem(static_cast<int16>(itemId)) == Package::kSuccess_AddItemRtn)
	{
		return true;
	}

	return false;
}

void BgPlayer::shutdown()
{
	if (pCon_)
	{
		pCon_->close();
	}
}

void BgPlayer::serializeResult(PacketBase& op, BgResultE bgResult, PacketBase& hotelop)
{
	hotelop.putInt32(getId());

	op.putInt32(getId());
	op.putInt32(team_);
	op.putUTF(name_);
	op.putInt32(killEnemyTimes_);
	if (bgResult == KDRAW_BGRESULT)
	{
		op.putInt32(2);

		hotelop.putInt32(3);
	}
	else if (bgResult == KBLACK_BGRESULT && team_ == BgUnit::kBlack_TEAM)
	{
		op.putInt32(5);

		hotelop.putInt32(1);
	}
	else if (bgResult == KWHITE_BGRESULT && team_ == BgUnit::kWhite_TEAM)
	{
		op.putInt32(5);

		hotelop.putInt32(1);
	}
	else
	{
		op.putInt32(1);

		hotelop.putInt32(2);
	}
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
	op.putInt32(maxhp_);
	op.putInt32(title_);
	op.putInt32(x_);
	op.putInt32(y_);
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
	LOG_TRACE << "BgPlayer::addBuf - playerId: " << this->getId()
							<< " name: " << name_
							<< " buf: " << buf->getId()
							<< " bufName: " << buf->getName();

	bufList_.push_back(buf);
	//  告诉客户端 xx 人中了buf
	PacketBase pb(client::OP_ADD_BUF, this->getId());
	pb.putInt32(buf->getId());
	broadMsg(pb);
	return true;
}

bool BgPlayer::hasSkill(int16 skillId)
{
	if (hasPet())
	{
		if (!sPetBaseMgr.checkPetId(petId_))
		{
			LOG_ERROR << " BgPlayer::hasSkill (not pet) - playerId: " << this->getId();
			return false;
		}
		const PetBase& petbase = sPetBaseMgr.getPetBaseInfo(petId_);
		return petbase.hasSkill(skillId);
	}
	return false;
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
			LOG_TRACE << "BgPlayer::canUseSkill, you can use skill - skillId: " << skillId
									<< " playerId: " << this->getId()
									<< " name: " << name_;
			return false;
		}
	}
	return true;
}

bool BgPlayer::useSkill(int16 skillId)
{
	int32 curTime = getCurTime();
	std::pair<std::map<int16, int32>::iterator,bool> res = useSkillMap_.insert(std::pair<int16, int32>(skillId, curTime));
	if (!res.second)
	{
		useSkillMap_[skillId] = curTime;
	}
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
	if (isDead())
	{
		attacker->incKillEnemyTime();
		//
		PacketBase op(client::OP_PET_DEAD, this->getId());
		broadMsg(op);
	}

	LOG_DEBUG <<  "BgPlayer::onHurt - playerId: " << this->getId()
							<< " name: " << name_
							<< " damage: " << damage
							<< " attacker: " << attacker->getId()
							<< " skillId:" << skill.skillId_;

	// 告诉客户端 你受到什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(0); //单元类型
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

	if (isDead())
	{
		PacketBase op(client::OP_PET_DEAD, this->getId());
		broadMsg(op);
	}
	LOG_DEBUG <<  "BgPlayer::onBufHurt - playerId: " << this->getId()
							<< " name: " << name_
							<< " damage: " << damage
							<< " bufid:" << buf.id_;

	// 告诉客户端 你受到什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(0); //单元类型
	pb.putInt32(1); // buf 伤害
	pb.putInt32(buf.id_);
	pb.putInt32(damage);
	broadMsg(pb);
}

void BgPlayer::incKillEnemyTime()
{
	this->killEnemyTimes_++;
}

void BgPlayer::fullHp()
{
	if (petId_ > 0)
	{
		if (!sPetBaseMgr.checkPetId(petId_))
		{
			return;
		}

		int lastHp = hp_;
		PetBase petbase = sPetBaseMgr.getPetBaseInfo(petId_);
		setPetId(petId_);
		setHp(petbase.hp_);

		if (pScene)
		{
			PacketBase op(client::OP_ADD_HP, this->getId());
			op.putInt32(hp_);
			op.putInt32(hp_ - lastHp);
			pScene->broadMsg(op);
		}
	}
}

void BgPlayer::runBuf(uint32 curTime)
{
	if (isDead()) return;

	//LOG_TRACE << "BgPlayer::runBuf - playerId: " << this->getId();

	std::list<Buf*>::iterator itList;
	for (itList = bufList_.begin(); itList != bufList_.end(); )
	{
		Buf* buf = *itList;
		assert(buf);
		buf->run(this, curTime);
		if (buf->waitDel())
		{
			itList = bufList_.erase(itList);
			LOG_TRACE << "BgPlayer::runBuf - remove buf - playerId: " << this->getId()
									<< " bufId: " << buf->getId()
									<< " name: " << buf->getName();

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

void BgPlayer::removeAllBuf()
{
	std::list<Buf*>::iterator itList;
	for (itList = bufList_.begin(); itList != bufList_.end(); ++itList)
	{
		Buf* buf = *itList;
		assert(buf);
		delete buf;
	}
	bufList_.clear();
}

void onEnter(PacketBase& pb)
{
	LOG_INFO << "onEnter";
}

void onExit(PacketBase& pb)
{
	LOG_INFO << "onExit";
}

#define BENGIN_MESSAGEHANDLE() switch (op) { \

#define INSERT_MESSAGEHANDLE(op, pb, handle) case op: \
																							onEnter(pb); \
																							handle(pb); \
																							onExit(pb); \
																							break; \

#define END_MESSAGEHANDLE() }



bool BgPlayer::onMsgHandler(PacketBase& pb)
{
	uint32 op = pb.getOP();
#if 1
	char hexop[10];
	snprintf(hexop, sizeof(hexop), "0x%X", op);
	LOG_INFO << " === START[msgHandler] - playerId:"  << getId()
							<< " playerName: " << name_
							<< " op: " << hexop
							<< " contentlen: " << pb.getContentLen();
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
		break;
	case game::OP_PICKUP_ITEM:
		onPickUpItem(pb);
		break;
	case game::OP_USE_ITEM:
		onUseItem(pb);
		break;
	default:
		break;
	}

	LOG_INFO << " === END[msgHandler] - playerId:"  << getId()
							<< " playerName: " << name_
							<< " op: " << hexop;

#else

	BENGIN_MESSAGEHANDLE()
	INSERT_MESSAGEHANDLE(game::OP_ENTER_BATTLE, onEnterBattle)
	END_MESSAGEHANDLE()
#endif

	return true;
}

// 消息处理函数

// 请求进入战场
void BgPlayer::onEnterBattle(PacketBase& pb)
{
	int16 bgId = static_cast<int16>(pb.getInt32());
	int16 teamValue = static_cast<int16>(pb.getInt32());

	if (! sBattlegroundMgr.checkBattlegroundId(bgId))
	{
		return;
	}

	BgUnit::TeamE team = (teamValue == BgUnit::kBlack_TEAM) ? BgUnit::kBlack_TEAM :BgUnit::kWhite_TEAM;

	PacketBase op(client::OP_ENTER_BATTLE, -1); // -1 代表是这个战场已经满人了
	op.putInt32(bgId);
	op.putInt32(teamValue);

	Battleground& bg = sBattlegroundMgr.getBattleground(bgId);
	if (bg.getState() == BattlegroundState::BGSTATE_EXIT )  // 战场在清理过程中, 不能进入这个战场哦
	{
		op.setParam(-2);
	}
	else if(bg.addBgPlayer(this, team))
	{
		//setBgId(bgId);
		op.setParam(0);
	}
	sendPacket(op);
}

void BgPlayer::onMove(PacketBase& pb)
{
	if (!pScene) return;

	// 广播给其他玩家
	pb.setOP(client::OP_MOVE);
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
		sendPacket(pb);
	}
}

void BgPlayer::onStand(PacketBase& pb)
{
	if (!operation_)
	{
		LOG_ERROR << "BgPlayer::onStand - you can operation - playerId: " << this->getId();
		return;
	}

	int16 x = static_cast<int16>(pb.getInt32());
	int16 y = static_cast<int16>(pb.getInt32());
	setX(x);
	setY(y);
}

void BgPlayer::onReqPlayerList(PacketBase& pb)
{
	if (!pScene) return;

	PacketBase op(client::OP_REQ_PLAYER_LIST, 0);
	std::map<int32, BgPlayer*>& playerMgr = pScene->getPlayerMgr();
	op.setParam(playerMgr.size());
	std::map<int32, BgPlayer*>::iterator iter;
	for(iter = playerMgr.begin(); iter != playerMgr.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			player->serialize(op);
		}
	}
	sendPacket(op);
}

void BgPlayer::onSelectPet(PacketBase& pb)
{
	if (!pScene) return;

	int16 petId = static_cast<int16>(pb.getParam());
	LOG_DEBUG << "BgPlayer::onSelectPet - petId:" << petId
							<< " playerId: " << this->getId()
							<< " playerName: " << name_;

	if (!sPetBaseMgr.checkPetId(petId))
	{
		return;
	}

	if (!isDead()) return;

	package_.clear();
	removeAllBuf();

	const PetBase& petbase = sPetBaseMgr.getPetBaseInfo(petId);
	setPetId(petId);
	setHp(petbase.hp_);
	setMaxHp(petbase.hp_);
	setUnitType(petbase.type_);
	init();

	pb.setOP(client::OP_SELCET_PET);
	pb.setParam(getId());
	pb.putInt32(petId);
	pb.putInt32(hp_);
	pScene->broadMsg(pb);
}

void BgPlayer::onUseSkill(PacketBase& pb)
{
	if (!pScene) return;

	if (!hasPet())
	{
		LOG_ERROR << "BgPlayer::onUseSkill - YOU DONOT HAS PET, CAN NOT USE SKILL - playerId: " << this->getId()
								<< " name:" << name_;
		return;
	}

	if (isDead())
	{
		LOG_ERROR << "BgPlayer::onUseSkill - YOU  PET is dead, CAN NOT USE SKILL - playerId: " << this->getId()
								<< " name:" << name_;
		return;
	}

	if (!operation_)
	{
		LOG_ERROR << "BgPlayer::onUseSkill  - you can operation - playerId: " << this->getId();
		return;
	}

	int16 skillId = static_cast<int16>(pb.getParam());
	int32 uintType = pb.getInt32();
	int32 playerId = pb.getInt32();
	int32 usePlayerId = pb.getInt32();
	int32 x = pb.getInt32();
	int32 y = pb.getInt32();

	PacketBase sendPb(client::OP_USE_SKILL, skillId);
	sendPb.putInt32(uintType);
	sendPb.putInt32(playerId);
	sendPb.putInt32(usePlayerId);
	sendPb.putInt32(x);
	sendPb.putInt32(y);
	broadMsg(sendPb);

	LOG_TRACE << "BgPlayer::onUseSkill  -  skillID: " << skillId
			<< " playerId: " << this->getId()
			<< " name: " << name_
			<< " uintType: " << uintType;

	int16 bgId = battlegroundId_;
	if (!sBattlegroundMgr.checkBattlegroundId(bgId))
	{
		return;
	}

	Battleground& bg = sBattlegroundMgr.getBattleground(bgId);

	BgUnit* target = bg.getTargetUnit(playerId, uintType);
	if (!target) return;
	// 敌方已经死啦 不能攻击他
	if (target->isDead()) return;
	// 不能攻击同一个队伍的人
	if (target->getTeam() == this->getTeam()) return;

	SkillHandler::onEmitSkill(skillId, this, target, pScene);
}

void BgPlayer::onExitBattle(PacketBase& pb)
{
	this->close();

	pb.setOP(client::OP_EXIT_BATTLE);
	sendPacket(pb);
}

void BgPlayer::onPickUpItem(PacketBase& pb)
{
	if (!pScene) return;

	int16 x = static_cast<int16>(pb.getInt32());
	int16 y = static_cast<int16>(pb.getInt32());
	pScene->pickUpItem(this, x, y);
	return;
}

void BgPlayer::onUseItem(PacketBase& pb)
{
	int16 itemId = static_cast<int16>(pb.getInt32());
	if (package_.hasItem(itemId))
	{
		package_.delItem(itemId);
		ItemHandler::onUseItem(itemId, this, NULL, pScene);
		//
		pb.setOP(client::OP_USE_ITEM);
		this->sendPacket(pb);
	}
}
