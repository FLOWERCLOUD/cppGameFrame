/*
 * pet.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/pet/pet.h>

Pet::Pet(int32 petId, int16 speed, int16 hp, int16 maxHp):
	petId_(petId),
	speed_(speed),
	hp_(hp),
	maxHp_(maxHp)
{
}

Pet::~Pet()
{
}

int32 Pet::getPetId()
{
	return petId_;
}

int16 Pet::getSpeed()
{
	return speed_;
}

int16 Pet::getHp()
{
	return hp_;
}

void Pet::setHp(int16 hp)
{
	hp_ = hp;
}

void Pet::addHp(int16 hp)
{
	hp_ += hp;
	hp_ = hp_ > maxHp_ ? maxHp_ : hp_;
}

void Pet::decHp(int16 hp)
{
	hp_ -= hp;
	hp_ =hp_ > 0 ? hp_ : 0;
}

void Pet::fullHp()
{
	hp_ = maxHp_;
}

