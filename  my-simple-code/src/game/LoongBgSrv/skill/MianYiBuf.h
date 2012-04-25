/*
 * MianYiBuf.h
 *
 *  Created on: 2012-3-29
 *    
 */

#ifndef GAME_MIANYIBUF_H_
#define GAME_MIANYIBUF_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/skill/Buf.h>

// 免疫buf
class MianYiBuf:public Buf
{
public:
	MianYiBuf(int16 bufId, int64 curTime, int64 bufferTime);
	virtual ~MianYiBuf();

	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);
};

#endif /* MIANYIBUF_H_ */
