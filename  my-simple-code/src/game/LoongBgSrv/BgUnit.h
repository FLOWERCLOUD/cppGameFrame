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
#include <game/LoongBgSrv/base/PetBase.h>

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

public:
	BgUnit();
	explicit BgUnit(UnitTypeE unitType);
	BgUnit(int32 unitId, UnitTypeE unitType);
	BgUnit(int32 unitId, UnitTypeE unitType, TeamE team);
	virtual ~BgUnit();

	virtual bool serialize(PacketBase& op);
	// 是否可以受到技能伤害
	virtual bool canSkillHurt();
	// 是否可以受到buf 伤害
	virtual bool canBufHurt();
	// 他已经中了这种buff
	virtual Buf* getBuf(int16 bufId);
	// 添加buf
	virtual bool addBuf(Buf* buf);
	virtual bool hasSkill(int16 skillId);
	virtual bool canUseSkill(int16 skillId, int32 cooldownTime);
	virtual bool useSkill(int16 skillId);
	// 伤害
	virtual void onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill);
	virtual void onBufHurt(BgUnit* me, int32 damage, const BufBase& buf);
	virtual void incKillEnemyTime();
	virtual void fullHp();
	virtual void sendPacket(PacketBase& op);
	typedef enum tagAlertCodeTypeE
	{
		NOMAL_ALERTCODETYPE	= 0,
		FLOW_ALERTCODETYPE = 1,
	} AlertCodeTypeE;
	virtual void alert(AlertCodeTypeE type, int32 code);

	bool isDead();

	int32 getId();
	int16 getX();
	int16 getY();
	TeamE getTeam();
	UnitTypeE getUnitType();
	int32 getHp();

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

	void setSkillHurtFlag(bool flag);
	void setBufHurtFlag(bool falg);

	void init();

	void setMaxHp(int32 maxhp)
	{
		maxhp_ = maxhp;
	}

	int32 getMaxHp()
	{
		return maxhp_;
	}
protected:
	int32 unittId_; //战场单元ID
	int16 x_; //战场单元x坐标点
	int16 y_; // 战场单元y坐标点
	int32 hp_; //战场单元的血量
	int32 maxhp_; //战场单元的最大血量
	bool operation_; //战场单元是否可以操作
	bool canSkillHurt_; //是否可以受到技能伤害
	bool canBufHurt_; //是否可以受到buff 伤害
	TeamE team_; //战场单元所在队伍
	UnitTypeE unitType_; //战场单元类型
};

#endif /* BGUNIT_H_ */
