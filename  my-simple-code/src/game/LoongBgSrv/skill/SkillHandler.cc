/*
 * SkillHandler.cc
 *
 *  Created on: 2012-3-22
 *    
 */

#include <game/LoongBgSrv/skill/SkillHandler.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <game/LoongBgSrv/base/SkillBaseMgr.h>
#include <game/LoongBgSrv/skill/BufHandler.h>
#include <game/LoongBgSrv/BgUnit.h>
#include <game/LoongBgSrv/Util.h>

#include <mysdk/base/Logging.h>

bool SkillHandler::onEmitSkill(int16 skillId, BgUnit* me, BgUnit* target, Scene* scene)
{
	LOG_DEBUG << "SkillHandler::onEmitSkill - skilld: " << skillId
							<< " me: " << me->getId()
							<< " target: " << target->getId();

	// 技能id 是否是正确的
	if (!sSkillBaseMgr.checkSkillId(skillId))
	{
		return false;
	}
    // 出招者 是不是有这个技能
	if (!me->hasSkill(skillId))
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - you has not  skill, skilld: " << skillId
								<< " me: " << me->getId()
								<< " target: " << target->getId();

		return false;
	}

	const SkillBase& skillbase = sSkillBaseMgr.getSkillBaseInfo(skillId);
	// 这个技能的冷却时间是不是还没有过
	int16 cooldownTime = skillbase.cooldownTime_;
	if (!me->canUseSkill(skillId, cooldownTime))
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - can not use skill, skilld: " << skillId
								<< " me: " << me->getId()
								<< " target: " << target->getId();

		return false;
	}
	me->useSkill(skillId);
	// 看看这个技能 能不能攻击到人家哦
	int32 attackDistance = (skillbase.attackDistance_  * skillbase.attackDistance_);
	if (getDistance(me, target) > attackDistance)
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - attackDistance too small, skilld: " << skillId
								<< " me: " << me->getId()
								<< " target: " << target->getId()
								<< " attackDistance: " << attackDistance;

		return false;
	}
	// 随机一个攻击值出来
	int32 maxAttackValue =  skillbase.maxAttackValue_ + 1;
	int32 minAttackValue = skillbase.minAttackValue_;
	int16 attackValue = static_cast<int16>(getRandomBetween(minAttackValue, maxAttackValue));

	int16 skillType = skillbase.type_;
	if (skillType == SkillBase::NOMAL) // 普通攻击要加成攻击力
	{
		attackValue = static_cast<int16>( attackValue * getBonusValue(me, target) / 100);
	}
	// 让攻击者伤害
	target->onHurt(me, attackValue, skillbase);

	// 附加buf
	int16 bufNum = skillbase.bufNum_;
	for (int16 i = 0; i < bufNum; i++)
	{
		int16 bufId = skillbase.bufList_[i];
		const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);
		int16 bufType = bufbase.type_;
		BufHandler::bufHandlers[bufType](bufId, attackValue, me, target, scene);
	}
	return true;
}
