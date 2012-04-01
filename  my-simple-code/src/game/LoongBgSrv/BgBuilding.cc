/*
 * BgBuilding.cc
 *
 *  Created on: 2012-3-29
 *    
 */

#include <game/LoongBgSrv/BgBuilding.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/scene/Scene.h>

BgBuilding::BgBuilding(int32 unitId, UnitTypeE unitType, TeamE team, Scene* pScene):
		BgUnit(unitId, unitType, team),
		pScene_(pScene)
{
}

BgBuilding::~BgBuilding()
{
}

void BgBuilding::init()
{
	setHp(sMaxHp);
}

bool BgBuilding::serialize(PacketBase& op)
{
	return true;
}

void BgBuilding::onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill)
{
	BgUnit::onHurt(attacker, damage, skill);
	if (hp_ < damage)
	{
		damage = hp_;
	}
	decHp(damage);
	// 告诉客户端 王座受什么伤害 伤害是多少
	if (!pScene_) return;
	PacketBase pb(client::OP_ON_HURT, 0);
	int32 type = this->getTeam() == BgUnit::kBlack_TEAM ? 1 : 2;
	pb.putInt32(type); //单元类型
	pb.putInt32(0); //技能伤害
	pb.putInt32(skill.skillId_);
	pb.putInt32(skill.type_);
	pb.putInt32(damage);
	pScene_->broadMsg(pb);
}
