/*
 * PetBase.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PETBASE_H_
#define GAME_PETBASE_H_

#include <mysdk/base/Common.h>

#include <string>
using namespace mysdk;
class PetBase
{
public:
	typedef enum tagPetTypeE
	{
		ONE_PETTYPE	= 0, // 傀儡巨人
		SECOND_PETTYPE = 1, //黑石大炮
		THREE_PETTYPE = 2, //地裂兽
	} PetTypeE;
public:
	static const int sMaxSkillNum = 3;
public:
	PetBase();
	~PetBase();

public:
	int16 petId_; //宠物的ID
	std::string petName_; // 宠物的名字
	int16 speed_; // 宠物的速度
	int16 hp_; // 宠物的血量
	PetTypeE type_; //宠物类型
    // 宠物的技能列表
	int16 skillNum_;
	int16 skillList_[sMaxSkillNum];
};

#endif /* PETBASE_H_ */
