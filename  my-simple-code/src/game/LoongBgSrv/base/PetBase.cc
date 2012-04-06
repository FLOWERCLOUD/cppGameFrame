/*
 * PetBase.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/base/PetBase.h>

PetBase::PetBase():
	petId_(0),
	speed_(0),
	hp_(0),
	skillNum_(0)
{
	for (int i = 0; i < sMaxSkillNum; i++)
	{
		skillList_[i] = 0;
	}
}

PetBase::~PetBase()
{
}

bool PetBase::serialize(PacketBase& op)
{
	op.putInt32(petId_);
	op.putInt32(hp_);
	return true;
}

bool PetBase::hasSkill(int16 skillId) const
{
	for (int i = 0; i < skillNum_; i++)
	{
		if (skillList_[i] == skillId)
		{
			return true;
		}
	}
	return false;
}
