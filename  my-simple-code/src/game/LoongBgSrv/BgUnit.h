/*
 * BgUnit.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_BGUNIT_H_
#define GAME_BGUNIT_H_

#include <mysdk/base/Common.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>
#include <game/LoongBgSrv/base/SkillBase.h>
#include <game/LoongBgSrv/base/BufBase.h>

using namespace mysdk;
class Buf;
class BgUnit
{
public:
	typedef enum tagTeamE
	{
		kNONE_TEAM = 0,
		kBlack_TEAM,  //暗黑军
		kWhite_TEAM,// 烈阳军

		kCOUNT_TEAM,
	} TeamE;

	typedef enum tagUnitTypeE
	{
		KNONE_UNITTYPE = 0,
		KONE_UNITTYPE	= 1, // 傀儡巨人
		KSECOND_UNITTYPE = 2,  //黑石大炮
		KTHREE_UNITTYPE	= 3, //地裂兽
		KFOUR_UNITTYPE = 4, //王座
	} UnitTypeE;

public:
	BgUnit();
	explicit BgUnit(UnitTypeE unitType);
	BgUnit(int32 unitId, UnitTypeE unitType);
	virtual ~BgUnit();

	virtual bool serialize(PacketBase& op);
	// 是否可以受到技能伤害
	virtual bool canSkillHurt();
	// 是否可以受到buf 伤害
	virtual bool canBufHurt();
	// 添加buf
	virtual bool addBuf(Buf* buf);
	virtual bool hasSkill(int16 skillId);
	virtual bool canUseSkill(int16 skillId, int32 cooldownTime);
	virtual bool useSkill(int16 skillId);
	// 伤害
	virtual void onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill);
	virtual void onBufHurt(BgUnit* me, int32 damage, const BufBase& buf);

	bool isDead();

	int32 getId();
	int16 getX();
	int16 getY();
	TeamE getTeam();
	UnitTypeE getUnitType();

	// 设置是否操作
	void setOperation(bool op);
	bool canOperation();

	void setX(int16 x);
	void setY(int16 y);
	void setTeam(TeamE team);
	void setUnitType(UnitTypeE unitType);
	void setHp(int32 hp);
	// 加血
	void addHp(int32 hp);
	// 减血
	void decHp(int32 hp);
protected:
	int32 unittId_; //战场单元ID
	int16 x_; //战场单元x坐标点
	int16 y_; // 战场单元y坐标点
	int32 hp_; //战场单元的血量
	bool operation_; //战场单元是否可以操作
	TeamE team_; //战场单元所在队伍
	UnitTypeE unitType_; //战场单元类型
};

#endif /* BGUNIT_H_ */
