/*
 * StaticData.h
 *
 *  Created on: 2012-4-19
 *    
 */

#ifndef GAME_STATICDATA_H_
#define GAME_STATICDATA_H_

#include <mysdk/base/Common.h>
using namespace mysdk;


static int32 heroIdList[] = {1, 2, 3};
static int32 skillList[4][2] = { {0, 0}, {1, 2}, {3, 4}, {5, 6} };

static int32 maxPos[] = {2910, 570};

static int32 blackStartPos[3][2] = { {0, 0}, {2638, 164}, {266, 414} };
static int32 whiteStartPos[3][2] = { {0, 0}, {2518, 284}, {386, 382} };

static int32 heroIdSpeed[] = {0, 1, 2, 3};

#endif /* STATICDATA_H_ */
