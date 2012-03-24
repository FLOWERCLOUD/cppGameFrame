/*
 * HunXuanBuf.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_HUNXUANBUF_H_
#define GAME_HUNXUANBUF_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/skill/Buf.h>

// 昏眩buf
class HunXuanBuf:public Buf
{
public:
	HunXuanBuf(int16 bufId, uint32 curTime, uint32 bufferTime);
	virtual ~HunXuanBuf();

	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);
};

#endif /* HUNXUANBUF_H_ */
