/*
 * ZhouShangBuf.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_ZHOUSHANGBUF_H_
#define GAME_ZHOUSHANGBUF_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/skill/Buf.h>
// 灼伤 buf
class ZhouShangBuf:public Buf
{
public:
	ZhouShangBuf(int16 bufId, uint32 curTime, uint32 bufferTime);
	virtual ~ZhouShangBuf();

	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);

};

#endif /* ZHOUSHANGBUF_H_ */
