/*
 * SkillHandler.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_SKILLHANDLER_H_
#define GAME_SKILLHANDLER_H_

#include <mysdk/base/Common.h>

using namespace mysdk;
class BgUnit;
class Scene;
class SkillHandler
{
public:
	static bool onEmitSkill(int16 skillId, BgUnit* me, BgUnit* target, Scene* sene);
};

#endif /* SKILLHANDLER_H_ */
