/*
 * PetBase.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PETBASE_H_
#define GAME_PETBASE_H_

#include <mysdk/base/Common.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <string>
using namespace mysdk;

typedef enum tagUnitTypeE
{
	KNONE_UNITTYPE = 0,
	KONE_UNITTYPE	= 1, // 傀儡巨人
	KSECOND_UNITTYPE = 2,  //黑石大炮
	KTHREE_UNITTYPE	= 3, //地裂兽
	KFOUR_UNITTYPE = 4, //王座
	KFIFTH_UNITTYPE = 5, //食人花
	KCOUNT_UNITTYPE,
} UnitTypeE;

class PetBase
{
public:
	static const int sMaxSkillNum = 3;
public:
	PetBase();
	~PetBase();

	bool serialize(PacketBase& op);
	bool hasSkill(int16 skillId) const;
public:
	int16 petId_; //宠物的ID
	std::string petName_; // 宠物的名字
	int16 speed_; // 宠物的速度
	int16 hp_; // 宠物的血量
	UnitTypeE type_; //战场单元类型
    // 宠物的技能列表
	int16 skillNum_;
	int16 skillList_[sMaxSkillNum];
};

#endif /* PETBASE_H_ */
