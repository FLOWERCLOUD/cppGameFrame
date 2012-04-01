/*
 * BufHandler.cc
 *
 *  Created on: 2012-3-22
 *    
 */

#include <game/LoongBgSrv/skill/BufHandler.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <game/LoongBgSrv/skill/Buf.h>
#include <game/LoongBgSrv/skill/HunXuanBuf.h>
#include <game/LoongBgSrv/skill/ZhouShangBuf.h>
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
    	// 自己不受溅射的伤害
    	if (player == me)
    	{
    		continue;
    	}
    	if (player->isDead())
    	{
    		continue;
    	}

    	int32 distance = getDistance(target, player);
    	if ( distance<= bufBase.paramList_[0])
    	{
    		int16 damage = static_cast<int16>(attackValue * bufBase.paramList_[1] / 100);
    		player->onBufHurt(player, damage, bufBase);
    	}
    	else if (distance <= bufBase.paramList_[2])
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
	uint32 curTime = getCurTime();
	uint32 bufferTime = curTime + seconds;
	Buf* buf = new HunXuanBuf(bufId, curTime, bufferTime);
	buf->onCacl(target);
	target->addBuf(buf);
	return true;
}
// 灼烧buff
static bool zhouShangBufHandler(int16 bufId, int16 attackValue, BgUnit* me, BgUnit* target, Scene* scene)
{
	const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);

	int16 seconds = bufbase.paramList_[1];
	uint32 curTime = getCurTime();
	uint32 bufferTime = curTime + seconds;
	Buf* buf = new ZhouShangBuf(bufId, curTime, bufferTime);
	target->addBuf(buf);
	return true;
}

BufHandler::bufHandler BufHandler::bufHandlers[] =
{
		defaultBufHandler,
		JianSheBufHandler,
		hunXuanBufHandler,
		zhouShangBufHandler,
		defaultBufHandler
};


