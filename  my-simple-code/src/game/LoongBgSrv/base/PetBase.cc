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
