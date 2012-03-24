/*
 * HunXuanBuf.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/skill/HunXuanBuf.h>

#include <assert.h>

HunXuanBuf::HunXuanBuf(int16 bufId, uint32 curTime, uint32 bufferTime):
	Buf(bufId, "昏眩", curTime, 0, bufferTime)
{

}

HunXuanBuf::~HunXuanBuf()
{
}

void HunXuanBuf::onDelete(BgUnit* me)
{
	assert(me);
	// 把玩家不能操作的指令移除
	me->setOperation(true);
}

void HunXuanBuf::onTick(BgUnit* me)
{

}

void HunXuanBuf::onCacl(BgUnit* me)
{
	assert(me);
	// 设置让玩家不能操作
	me->setOperation(false);
}
