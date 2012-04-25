/*
 * AddSpeedBuf.h
 *
 *  Created on: 2012-4-23
 *    
 */

#ifndef GAME_ADDSPEEDBUF_H_
#define GAME_ADDSPEEDBUF_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/skill/Buf.h>

// 加速度buf
class AddSpeedBuf:public Buf
{
public:
	AddSpeedBuf(int16 bufId, int64 curTime, int64 bufferTime);
	virtual ~AddSpeedBuf();

	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);
};

#endif /* ADDSPEEDBUF_H_ */
