/*
 * BgBuilding.h
 *
 *  Created on: 2012-3-29
 *    
 */

#ifndef GAME_BGBUILDING_H_
#define GAME_BGBUILDING_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BgUnit.h>
// 王座
class Scene;
class BgBuilding:public BgUnit
{
public:
	static const int32 sMaxHp = 5000;
public:
	BgBuilding(int32 unitId, UnitTypeE unitType, TeamE team, Scene* pScene);
	virtual ~BgBuilding();

	void init();

public:
	// 来自父类
	virtual bool serialize(PacketBase& op);
	virtual void onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill);
private:
	Scene* pScene_;
};

#endif /* BGBUILDING_H_ */
