/*
 * ZhouShangBuf.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/skill/ZhouShangBuf.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <mysdk/base/Logging.h>

ZhouShangBuf::ZhouShangBuf(int16 bufId, int64 curTime, int64 bufferTime):
	Buf(bufId, "灼伤", curTime, 1000, bufferTime)
{

}

ZhouShangBuf::~ZhouShangBuf()
{

}


void ZhouShangBuf::onDelete(BgUnit* me)
{

}

void ZhouShangBuf::onTick(BgUnit* me)
{
	LOG_TRACE << " ZhouShangBuf::onTick - playerId: " << me->getId();
	if (me->canSkillHurt())
	{
		const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId_);
		int16 decHp = bufbase.paramList_[0];

		me->onBufHurt(me, decHp, bufbase);
	}
	else
	{
		LOG_TRACE << " ZhouShangBuf::onTick - playerId: " << me->getId()
								<< " can skill hurt!!";
		waitDel_ = true;
	}
}

void ZhouShangBuf::onCacl(BgUnit* me)
{
}
