/*
 * MianYiBuf.cc
 *
 *  Created on: 2012-3-29
 *    
 */

#include <game/LoongBgSrv/skill/MianYiBuf.h>

#include <assert.h>

MianYiBuf::MianYiBuf(int16 bufId, int64 curTime, int64 bufferTime):
	Buf(bufId, "免疫", curTime, 0, bufferTime)
{

}

MianYiBuf::~MianYiBuf()
{
}

void MianYiBuf::onDelete(BgUnit* me)
{
	assert(me);
	me->setSkillHurtFlag(true);
}

void MianYiBuf::onTick(BgUnit* me)
{

}

void MianYiBuf::onCacl(BgUnit* me)
{
	assert(me);
	me->setSkillHurtFlag(false);
}
