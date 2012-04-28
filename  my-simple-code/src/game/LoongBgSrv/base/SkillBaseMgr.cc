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
//	init();
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

bool SkillBaseMgr::init(TestDatabaseWorkerPool& databaseWorkPool)
{
	const char* sql = "select skillid, skillname, minAttackValue, maxAttackValue, attackDistance, cooldownTime, skilltype, passive, skillbuff from skill";
	ResultSet* res = databaseWorkPool.query(sql);
	if (!res) return false;

	while (res->nextRow())
	{
		const Field* field = res->fetch();
		skillBaseList_[curSkillBaseNum_].skillId_ = field[0].getInt16();
		skillBaseList_[curSkillBaseNum_].skillName_ = field[1].getString();
		skillBaseList_[curSkillBaseNum_].minAttackValue_ = field[2].getInt16();
		skillBaseList_[curSkillBaseNum_].maxAttackValue_ = field[3].getInt16();
		skillBaseList_[curSkillBaseNum_].attackDistance_  = field[4].getInt16();
		skillBaseList_[curSkillBaseNum_].cooldownTime_ = field[5].getInt16();
		skillBaseList_[curSkillBaseNum_].type_ = field[6].getInt16();
		skillBaseList_[curSkillBaseNum_].passive_ = field[7].getInt16();
		ColonBuf buf(field[8].getCString());
		int32 bufNum = skillBaseList_[curSkillBaseNum_].bufNum_ = buf.GetShort();
		for (int32 i = 0; i < bufNum; i++)
		{
			skillBaseList_[curSkillBaseNum_].bufList_[i] = buf.GetShort();
		}
		curSkillBaseNum_++;
	}
	delete res;

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
