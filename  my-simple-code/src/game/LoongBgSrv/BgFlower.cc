/*
 * BgFlower.cc
 *
 *  Created on: 2012-4-23
 *    
 */

#include <game/LoongBgSrv/BgFlower.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/scene/Scene.h>
#include <game/LoongBgSrv/skill/Buf.h>
#include <game/LoongBgSrv/skill/SkillHandler.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/Util.h>

BgFlower::BgFlower(int32 unitId, UnitTypeE unitType, TeamE team, Scene* pScene):
		BgUnit(unitId, unitType, team),
		plantTimes_(0),
		useSkillTimes_(0),
		pScene_(pScene)
{
	setHp(sMaxHp);
	setMaxHp(sMaxHp);
}

BgFlower::~BgFlower()
{
	removeAllBuf();
}

void BgFlower::init()
{

}

void BgFlower::run(int64 curTime)
{
	if (isDead()) return;
	runBuf(curTime);
	if (operation_)
	{
		onAIHandle(curTime);
	}
}

void BgFlower::shutdown()
{

}

bool BgFlower::serialize(PacketBase& op)
{
	op.putInt32(this->getId());
	op.putInt32(this->getTeam());
	op.putInt32(this->unitType_);
	op.putInt32(x_);
	op.putInt32(y_);
	op.putInt32(hp_);
	return true;
}

bool BgFlower::canSkillHurt()
{
	return true;
}

bool BgFlower::canBufHurt()
{
	return true;
}

Buf* BgFlower::getBuf(int16 bufId)
{
	std::list<Buf*>::iterator itList;
	for (itList = bufList_.begin(); itList != bufList_.end(); ++itList)
	{
		Buf* buf = *itList;
		if (buf && buf->getId() == bufId)
		{
			return buf;
		}
	}
	return NULL;
}

bool BgFlower::addBuf(Buf* buf)
{
	bufList_.push_back(buf);

	//  告诉客户端 xx 人中了buf
	PacketBase pb(client::OP_ADD_BUF, this->getId());
	pb.putInt32(this->getId());
	pb.putInt32(3);
	pb.putInt32(buf->getId());
	pb.putInt32(x_);
	pb.putInt32(y_);
	broadMsg(pb);

	return true;
}

void BgFlower::onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill)
{
	BgUnit::onHurt(attacker, damage, skill);
	if (hp_ < damage)
	{
		damage = hp_;
	}
	decHp(damage);

	// 告诉客户端 食人花受什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(3); //单元类型
	pb.putInt32(0); //技能伤害
	pb.putInt32(skill.skillId_);
	pb.putInt32(skill.type_);
	pb.putInt32(damage);
	broadMsg(pb);
}

void BgFlower::onBufHurt(BgUnit* me, int32 damage, const BufBase& buf)
{
	BgUnit::onBufHurt(me, damage, buf);
	if (hp_ < damage)
	{
		damage = hp_;
	}
	decHp(damage);

	// 告诉客户端 你受到什么伤害 伤害是多少
	PacketBase pb(client::OP_ON_HURT, this->getId());
	pb.putInt32(3); //单元类型
	pb.putInt32(1); // buf 伤害
	pb.putInt32(buf.id_);
	pb.putInt32(damage);
	broadMsg(pb);
}

bool BgFlower::canUseSkill(int16 skillId, int32 cooldownTime)
{
	return true;
}

bool BgFlower::hasSkill(int16 skillId)
{
	return true;
}

void BgFlower::removeAllBuf()
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

void BgFlower::broadMsg(PacketBase& op)
{
	if (pScene_)
	{
		pScene_->broadMsg(op);
	}
}

void BgFlower::onAIHandle(int64 curTime)
{
	static int16 skillId = 7; //食人花技能
	static int32 maxDistance = 120 * 120; // 120 像素

	if (curTime - useSkillTimes_ > sUseSkillTimes)
	{
		bool flag = false; //是否已经使用了技能了
		std::map<int32, BgPlayer*>& playerMgr = pScene_->getPlayerMgr();
		std::map<int32,BgPlayer*>::iterator it;
		for (it = playerMgr.begin(); it != playerMgr.end(); ++it)
		{
			BgPlayer* player = it->second;
			if (!player) continue;
			if (player->isDead())
			{
				continue;
			}
			if (player->getTeam() == this->getTeam())
			{
				continue;
			}
			int32 distance = getDistance(this, player);
			if (distance <= maxDistance)
			{
				// 告诉客户端 食人花 使用了xx 技能
				PacketBase sendPb(client::OP_USE_SKILL, skillId);
				sendPb.putInt32(0);
				sendPb.putInt32(player->getId());
				sendPb.putInt32(3);
				sendPb.putInt32(this->getId());
				sendPb.putInt32(this->getX());
				sendPb.putInt32(this->getY());
				broadMsg(sendPb);

				LOG_TRACE << "onAIHandle -- attackId: " << player->getId();
				SkillHandler::onEmitSkill(skillId, this, player, pScene_);
				flag = true;
				break;
			}
		}

		if (!flag)
		{
			if (this->getTeam() == BgUnit::kBlack_TEAM)
			{
				int32 distance = getDistance(this, &pScene_->getWhiteBuilding());
				if (distance <= maxDistance) // 120 像素
				{
					// 告诉客户端 食人花 使用了xx 技能
					PacketBase sendPb(client::OP_USE_SKILL, skillId);
					sendPb.putInt32(2);
					sendPb.putInt32(0);
					sendPb.putInt32(3);
					sendPb.putInt32(this->getId());
					sendPb.putInt32(this->getX());
					sendPb.putInt32(this->getY());
					broadMsg(sendPb);

					SkillHandler::onEmitSkill(skillId, this, &pScene_->getWhiteBuilding(), pScene_);
					flag = true;
				}
			}
			else if (this->getTeam() == BgUnit::kWhite_TEAM)
			{
				int32 distance = getDistance(this, &pScene_->getBlackBuilding());
				if (distance <= maxDistance) // 120 像素
				{
					// 告诉客户端 食人花 使用了xx 技能
					PacketBase sendPb(client::OP_USE_SKILL, skillId);
					sendPb.putInt32(1);
					sendPb.putInt32(0);
					sendPb.putInt32(3);
					sendPb.putInt32(this->getId());
					sendPb.putInt32(this->getX());
					sendPb.putInt32(this->getY());
					broadMsg(sendPb);

					SkillHandler::onEmitSkill(skillId, this, &pScene_->getBlackBuilding(), pScene_);
					flag = true;
				}
			}
		}

		if (flag)
		{
			useSkillTimes_ = curTime;
		}
	}
}

void BgFlower::runBuf(int64 curTime)
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
			pb.putInt32(this->getId());
			pb.putInt32(3);
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

