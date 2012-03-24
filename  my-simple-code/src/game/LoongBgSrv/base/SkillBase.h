/*
 * SkillBase.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_SKILLBASE_H_
#define GAME_SKILLBASE_H_

#include <mysdk/base/Common.h>

#include <string>
using namespace mysdk;
class SkillBase
{
public:
	static const int sMaxBufNum = 2;

public:
	typedef enum tagSkillTypeE
	{
		NOMAL	= 0,
		MAGIC	= 1
	}SkillTypeE;
public:
	SkillBase();
	~SkillBase();

public:
	int16 skillId_; //技能ID
	std::string skillName_; // 技能的名字
	int16 maxAttackValue_; //技能最大攻击值
	int16 minAttackValue_; // 技能最小攻击力
	int16 attackDistance_; // 技能攻击距离 （单位_像素）
	int16 cooldownTime_; // 冷却时间 单位（秒）
	int16 type_; // 技能的类型，0为物理攻击，1为技能攻击
	int16 passive_; // 技能属性，0为主动，1为被动
	int16 bufNum_;
	int16 bufList_[sMaxBufNum];
};

#endif /* SKILLBASE_H_ */
