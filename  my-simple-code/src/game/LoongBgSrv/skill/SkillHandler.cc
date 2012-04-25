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
#include <game/LoongBgSrv/ErrorCode.h>
#include <game/LoongBgSrv/Util.h>

#include <mysdk/base/Logging.h>

bool SkillHandler::onEmitSkill(int16 skillId, BgUnit* me, BgUnit* target, Scene* scene)
{
	LOG_TRACE << "SkillHandler::onEmitSkill - skilld: " << skillId
							<< " me: " << me->getId()
							<< " target: " << target->getId()
							<< " teamType: " << target->getTeam()
							<< " unitType: " << target->getUnitType();

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
    // 对方是否有免疫技能
	if (!target->canSkillHurt())
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - target  can skill hurt, skilld: " << skillId
								<< " me: " << me->getId()
								<< " target: " << target->getId();

		me->alert(BgUnit::FLOW_ALERTCODETYPE, ErrorCode::BG_CANNOT_HURT);
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
	/*
	// 看看这个技能 能不能攻击到人家哦
	int32 attackDistance = (skillbase.attackDistance_  * skillbase.attackDistance_);
	int32 distance = getDistance(me, target);
	if ( distance > attackDistance + 1200)
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - attackDistance too small, skilld: " << skillId
								<< " me: " << me->getId()
								<< " me x: " << me->getX()
								<< " me y:" << me->getY()
								<< " target: " << target->getId()
								<< " target x: " << target->getX()
								<< " target y: " << target->getY()
								<< " distance: " << distance
								<< " attackDistance: " << attackDistance;

		me->alert(BgUnit::FLOW_ALERTCODETYPE, ErrorCode::BG_SKILL_TOO_MUCH_DISTANCE);
		return false;
	}
	*/
	// 随机一个攻击值出来
	int32 maxAttackValue =  skillbase.maxAttackValue_;
	int32 minAttackValue = skillbase.minAttackValue_;
	int16 attackValue = static_cast<int16>(getRandomBetween(minAttackValue, maxAttackValue));

	LOG_DEBUG <<  "BgPlayer::onHurt - playerId: " << me->getId()
							<< " damage: " << attackValue
							<< " attacker: " << target->getId()
							<< " skillId:" << skillId;

	int16 skillType = skillbase.type_;
	if (skillType == SkillBase::NOMAL) // 普通攻击要加成攻击力
	{
		attackValue = static_cast<int16>( (attackValue * getBonusValue(me, target)) / 100);
	}

	LOG_DEBUG <<  "BgPlayer::onHurt - playerId: " << me->getId()
							<< " damage: " << attackValue
							<< " attacker: " << target->getId()
							<< " skillId:" << skillId;

	// 让攻击者伤害
	target->onHurt(me, attackValue, skillbase);

	if (!target->canBufHurt())
	{
		LOG_DEBUG << "SkillHandler::onEmitSkill - can buf hurt, skilld: " << skillId
								<< " me: " << me->getId()
								<< " target: " << target->getId();

		return true;
	}
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
