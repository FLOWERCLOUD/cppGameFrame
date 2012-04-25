/*
 * AddSpeedBuf.cc
 *
 *  Created on: 2012-4-23
 *    
 */

#include <game/LoongBgSrv/skill/AddSpeedBuf.h>

#include <assert.h>

AddSpeedBuf::AddSpeedBuf(int16 bufId, int64 curTime, int64 bufferTime):
	Buf(bufId, "提速", curTime, 0, bufferTime)
{

}

AddSpeedBuf::~AddSpeedBuf()
{
}

void AddSpeedBuf::onDelete(BgUnit* me)
{
}

void AddSpeedBuf::onTick(BgUnit* me)
{

}

void AddSpeedBuf::onCacl(BgUnit* me)
{
}
