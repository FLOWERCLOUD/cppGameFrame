/*
 * BgUnit.cc
 *
 *  Created on: 2012-3-23
 *    
 */

#include <game/LoongBgSrv/BgUnit.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/skill/Buf.h>

BgUnit::BgUnit():
	unittId_(0),
	x_(0),
	y_(0),
	hp_(0),
	operation_(true),
	team_(kNONE_TEAM),
	unitType_(KNONE_UNITTYPE)
{
}

BgUnit::BgUnit(UnitTypeE unitType):
		unittId_(0),
		x_(0),
		y_(0),
		hp_(0),
		operation_(true),
		team_(kNONE_TEAM),
		unitType_(unitType)
{

}

BgUnit::BgUnit(int32 unitId, UnitTypeE unitType):
		unittId_(unitId),
		x_(0),
		y_(0),
		hp_(0),
		operation_(true),
		team_(kNONE_TEAM),
		unitType_(unitType)
{

}

BgUnit::BgUnit(int32 unitId, UnitTypeE unitType, TeamE team):
				unittId_(unitId),
				x_(0),
				y_(0),
				hp_(0),
				operation_(true),
				team_(team),
				unitType_(unitType)
{

}

BgUnit::~BgUnit()
{

}

bool BgUnit::serialize(PacketBase& op)
{
	return false;
}

// 是否可以受到技能伤害
bool BgUnit::canSkillHurt()
{
	return true;
}
// 是否可以受到buf 伤害
bool BgUnit::canBufHurt()
{
	return false;
}

bool BgUnit::addBuf(Buf* buf)
{
	return false;
}

bool BgUnit::isDead()
{
	return hp_ <= 0;
}

int32 BgUnit::getId()
{
	return unittId_;
}

int16 BgUnit::getX()
{
	return x_;
}

int16 BgUnit::getY()
{
	return y_;
}

BgUnit::TeamE BgUnit::getTeam()
{
	return team_;
}

UnitTypeE BgUnit::getUnitType()
{
	return unitType_;
}

int32 BgUnit::getHp()
{
	return hp_;
}

void BgUnit::setX(int16 x)
{
	x_ = x;
}

void BgUnit::setY(int16 y)
{
	y_ = y;
}

void BgUnit::setTeam(TeamE team)
{
	team_ = team;
}

void BgUnit::setUnitType(UnitTypeE unitType)
{
	unitType_ = unitType;
}

void BgUnit::setHp(int32 hp)
{
	hp_ = hp;
}

// 加血
void BgUnit::addHp(int32 hp)
{
	hp_ += hp;
}
// 减血
void BgUnit::decHp(int32 hp)
{
	hp_ -= hp;
}

void BgUnit::setOperation(bool op)
{
	operation_ = op;
}

bool BgUnit::canOperation()
{
	return operation_;
}

bool BgUnit::hasSkill(int16 skillId)
{
	return false;
}

bool BgUnit::canUseSkill(int16 skillId, int32 cooldownTime)
{
	return false;
}

bool BgUnit::useSkill(int16 skillId)
{
	return false;
}

// 伤害
void BgUnit::onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill)
{
}

void BgUnit::onBufHurt(BgUnit* me, int32 damage, const BufBase& buf)
{

}

void BgUnit::incKillEnemyTime()
{

}

void BgUnit::fullHp()
{

}

void BgUnit::sendPacket(PacketBase& op)
{

}

void BgUnit::alert(AlertCodeTypeE type, int32 code)
{
	PacketBase op(client::OP_ALERT_CODE, type);
	op.putInt32(code);
	sendPacket(op);
}
