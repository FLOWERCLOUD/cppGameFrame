/*
 * MianYiBuf.cc
 *
 *  Created on: 2012-3-29
 *    
 */

#include <game/LoongBgSrv/skill/MianYiBuf.h>

#include <assert.h>

MianYiBuf::MianYiBuf(int16 bufId, uint32 curTime, uint32 bufferTime):
	Buf(bufId, "免疫", curTime, 0, bufferTime)
{

}

MianYiBuf::~MianYiBuf()
{
}

void MianYiBuf::onDelete(BgUnit* me)
{
	assert(me);
}

void MianYiBuf::onTick(BgUnit* me)
{

}

void MianYiBuf::onCacl(BgUnit* me)
{
	assert(me);
}
