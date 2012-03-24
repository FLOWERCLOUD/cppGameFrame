/*
 * pet.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PET_H_
#define GAME_PET_H_

#include <mysdk/base/Common.h>

class Pet
{
public:
	Pet(int32 petId, int16 speed, int16 hp, int16 maxHp);
	~Pet();

	int32 getPetId();
	int16 getSpeed();
	int16 getHp();
	void setHp(int16 hp);
	void addHp(int16 hp); // 加血
	void decHp(int16 hp); // 减血
	void fullHp(); //满血
private:
	int32 petId_; //宠物ID
	int16 speed_; // 宠物的速度
	int16 hp_; // 宠物的血量
	int16 maxHp_; //该宠物的最大血量

private:
	DISALLOW_COPY_AND_ASSIGN(Pet);
};

#endif /* PET_H_ */
