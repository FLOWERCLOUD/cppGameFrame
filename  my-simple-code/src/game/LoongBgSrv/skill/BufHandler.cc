/*
 * BufHandler.cc
 *
 *  Created on: 2012-3-22
 *    
 */

#include <game/LoongBgSrv/skill/BufHandler.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <game/LoongBgSrv/skill/Buf.h>
#include <game/LoongBgSrv/skill/AddSpeedBuf.h>
#include <game/LoongBgSrv/skill/HunXuanBuf.h>
#include <game/LoongBgSrv/skill/ZhouShangBuf.h>
#include <game/LoongBgSrv/skill/MianYiBuf.h>
#include <game/LoongBgSrv/scene/Scene.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/Util.h>

#include <map>

static bool defaultBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	return false;
}
// 溅射buff
static bool JianSheBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufBase = sBufBaseMgr.getBufBaseInfo(bufId);

	std::map<int32, BgPlayer*>& playerMgr = scene->getPlayerMgr();
    std::map<int32,BgPlayer*>::iterator it;
    for (it = playerMgr.begin(); it != playerMgr.end(); ++it)
    {
    	BgPlayer* player = it->second;
    	if (!player) continue;
    	// 自己不受溅射的伤害
    	if (player == me)
    	{
    		continue;
    	}
    	if (player->isDead())
    	{
    		continue;
    	}

    	if (player->getTeam() == me->getTeam())
    	{
    		continue;
    	}

    	int32 distance = getDistance(target, player);
    	if ( distance<= bufBase.paramList_[0] * bufBase.paramList_[0])
    	{
    		int16 damage = static_cast<int16>(attackValue * bufBase.paramList_[1] / 100);
    		player->onBufHurt(player, damage, bufBase);
    	}
    	else if (distance <= bufBase.paramList_[2] * bufBase.paramList_[2])
    	{
    		int16 damage = static_cast<int16>(attackValue * bufBase.paramList_[3] / 100);
    		player->onBufHurt(player, damage, bufBase);
    	}
    }

	return true;
}
// 昏眩buff
static bool hunXuanBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);
	int16 seconds = bufbase.paramList_[0];
	int64 curTime = getCurTime();
	int64 bufferTime = curTime + seconds;
	Buf* buf = target->getBuf(bufId);
	if (!buf)
	{
		Buf* hunXuanBuf = new HunXuanBuf(bufId, curTime, bufferTime);
		if (hunXuanBuf)
		{
			hunXuanBuf->onCacl(target);
			target->addBuf(hunXuanBuf );
		}
	}
	else
	{
		buf->setLastTime(curTime);
		buf->setBufferTime(bufferTime);
	}
	return true;
}
// 灼烧buff
static bool zhouShangBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);

	int16 seconds = bufbase.paramList_[1];
	int64 curTime = getCurTime();
	int64 bufferTime = curTime + seconds;
	// 以前他就中了这个buff的话 就替换原来的那个buff 如果没有中过 就加一个buff
	Buf* buf = target->getBuf(bufId);
	if (!buf)
	{
		target->addBuf(new ZhouShangBuf(bufId, curTime, bufferTime) );
	}
	else
	{
		buf->setLastTime(curTime);
		buf->setBufferTime(bufferTime);
	}
	return true;
}

// 免疫buff
static bool mianYiBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);

	int16 seconds = bufbase.paramList_[0];
	int64 curTime = getCurTime();
	int64 bufferTime = curTime + seconds;
	// 以前他就中了这个buff的话 就替换原来的那个buff 如果没有中过 就加一个buff
	Buf* buf = me->getBuf(bufId);
	if (!buf)
	{
		Buf* mianYiBuf = new MianYiBuf(bufId, curTime, bufferTime);
		if (mianYiBuf)
		{
			mianYiBuf->onCacl(me);
			me->addBuf( mianYiBuf );
		}
	}
	else
	{
		buf->setLastTime(curTime);
		buf->setBufferTime(bufferTime);
	}
	return true;
}

// 提速buff
static bool addSpeedBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);

	int16 seconds = bufbase.paramList_[0];
	int64 curTime = getCurTime();
	int64 bufferTime = curTime + seconds;
	// 以前他就中了这个buff的话 就替换原来的那个buff 如果没有中过 就加一个buff
	Buf* buf = me->getBuf(bufId);
	if (!buf)
	{
		me->addBuf(new AddSpeedBuf(bufId, curTime, bufferTime) );
	}
	else
	{
		buf->setLastTime(curTime);
		buf->setBufferTime(bufferTime);
	}
	return true;
}

BufHandler::bufHandler BufHandler::bufHandlers[] =
{
		defaultBufHandler,
		hunXuanBufHandler,
		zhouShangBufHandler,
		JianSheBufHandler,
		mianYiBufHandler,
		addSpeedBufHandler,
		defaultBufHandler
};


