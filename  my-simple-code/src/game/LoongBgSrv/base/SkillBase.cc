/*
 * SkillBase.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/base/SkillBase.h>

SkillBase::SkillBase():
	skillId_(0),
	skillName_("未知"),
	maxAttackValue_(0),
	minAttackValue_(0),
	attackDistance_(0),
	cooldownTime_(0),
	type_(NOMAL),
	passive_(0),
	bufNum_(0)
{
	for (int i = 0; i < sMaxBufNum; i++)
	{
		bufList_[i] = 0;
	}
}

SkillBase::~SkillBase()
{
}

