/*
 * ItemHandler.h
 *
 *  Created on: 2012-3-29
 *    
 */

#ifndef GAME_ITEMHANDLER_H_
#define GAME_ITEMHANDLER_H_

#include <mysdk/base/Common.h>

using namespace mysdk;
class Scene;
class BgUnit;
class ItemHandler
{
public:
	static bool onUseItem(int16 itemId, BgUnit* me, BgUnit* target, Scene* sene);
};


#endif /* ITEMHANDLER_H_ */
