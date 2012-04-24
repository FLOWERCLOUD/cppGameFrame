/*
 * BgFlower.h
 *
 *  Created on: 2012-4-23
 *    
 */

#ifndef GAME_BGFLOWER_H_
#define GAME_BGFLOWER_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BgUnit.h>

#include <list>
// 食人花
class Scene;
class BgFlower:public BgUnit
{
public:
	static const int32 sMaxHp = 300;
	static const uint32 sUseSkillTimes = 2000; // 2s
public:
	BgFlower(int32 unitId, UnitTypeE unitType, TeamE team, Scene* pScene);
	virtual ~BgFlower();

	void init();
	void run(uint32 curTime);
	void shutdown();

	void setPlantTimes(uint32 plantTime)
	{
		plantTimes_ = plantTime;
	}
	uint32 getPlantTimes()
	{
		return plantTimes_;
	}
public:
	// 来自父类
	virtual bool serialize(PacketBase& op);
	virtual bool canSkillHurt();
	virtual bool canBufHurt();
	virtual Buf* getBuf(int16 bufId);
	virtual bool addBuf(Buf* buf);
	virtual void onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill);
	virtual void onBufHurt(BgUnit* me, int32 damage, const BufBase& buf);
	virtual bool canUseSkill(int16 skillId, int32 cooldownTime);
	virtual bool hasSkill(int16 skillId);

private:
	void runBuf(uint32 curTime);
	void removeAllBuf();

	void broadMsg(PacketBase& op);
	void onAIHandle(uint32 curTime);
private:
	uint32 plantTimes_;
	uint32 useSkillTimes_;
	std::list<Buf*> bufList_; //食人花的buf 列表
	Scene* pScene_;
};

#endif /* BGFLOWER_H_ */
