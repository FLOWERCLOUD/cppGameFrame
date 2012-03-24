/*
 * BufHandler.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_BUFHANDLER_H_
#define GAME_BUFHANDLER_H_

#include <mysdk/base/Common.h>
using namespace mysdk;

class BgUnit;
class Scene;
class BufHandler
{
public:
	typedef bool (*bufHandler)(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene);
public:
	static int const sMaxBufHandlerNum = 5;
public:
	static bufHandler bufHandlers[ ];
};

#endif /* BUFHANDLER_H_ */
