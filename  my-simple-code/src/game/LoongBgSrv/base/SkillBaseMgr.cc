/*
 * SkillBaseMgr.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/base/SkillBaseMgr.h>

SkillBaseMgr::SkillBaseMgr():
	curSkillBaseNum_(0)
{

}

SkillBaseMgr::~SkillBaseMgr()
{
}

bool SkillBaseMgr::init()
{
	skillBaseList_[curSkillBaseNum_].skillId_ = 1;
	skillBaseList_[curSkillBaseNum_].skillName_ = "傀儡巨人普通攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 80;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  120;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 0;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 2000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::NOMAL;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	curSkillBaseNum_++;
	skillBaseList_[curSkillBaseNum_].skillId_ = 2;
	skillBaseList_[curSkillBaseNum_].skillName_ = "傀儡巨人技能攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 80;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  120;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 0;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 2000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::MAGIC;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	skillBaseList_[curSkillBaseNum_].bufNum_ = 1;
	skillBaseList_[curSkillBaseNum_].bufList_[0] = 1;
	curSkillBaseNum_++;
	skillBaseList_[curSkillBaseNum_].skillId_ = 3;
	skillBaseList_[curSkillBaseNum_].skillName_ = "黑石大炮普通攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 160;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  200;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 450;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 2000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::NOMAL;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	curSkillBaseNum_++;
	skillBaseList_[curSkillBaseNum_].skillId_ = 4;
	skillBaseList_[curSkillBaseNum_].skillName_ = "黑石大炮技能攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 160;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  200;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 450;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 3000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::MAGIC;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	skillBaseList_[curSkillBaseNum_].bufNum_ = 1;
	skillBaseList_[curSkillBaseNum_].bufList_[0] = 2;
	curSkillBaseNum_++;
	skillBaseList_[curSkillBaseNum_].skillId_ = 5;
	skillBaseList_[curSkillBaseNum_].skillName_ = "地裂兽普通攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 70;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  100;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 0;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 1000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::NOMAL;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	curSkillBaseNum_++;
	skillBaseList_[curSkillBaseNum_].skillId_ = 6;
	skillBaseList_[curSkillBaseNum_].skillName_ = "地裂兽技能攻击";
	skillBaseList_[curSkillBaseNum_].minAttackValue_ = 70;
	skillBaseList_[curSkillBaseNum_].maxAttackValue_ =  100;
	skillBaseList_[curSkillBaseNum_].attackDistance_ = 0;
	skillBaseList_[curSkillBaseNum_].cooldownTime_ = 4000;
	skillBaseList_[curSkillBaseNum_].type_ = SkillBase::MAGIC;
	skillBaseList_[curSkillBaseNum_].passive_ = 0;
	skillBaseList_[curSkillBaseNum_].bufNum_ = 1;
	skillBaseList_[curSkillBaseNum_].bufList_[0] = 3;

	return true;
}

void SkillBaseMgr::shutdown()
{

}

const SkillBase& SkillBaseMgr::getSkillBaseInfo(int16 skillId) const
{
	static SkillBase null;
	if (skillId <= 0 || skillId > sMaxSkillNum) return null;

	return skillBaseList_[skillId - 1];
}

bool SkillBaseMgr::checkSkillId(int16 skillId)
{
	if (skillId > 0 && skillId <= curSkillBaseNum_) return true;

	return false;
}
