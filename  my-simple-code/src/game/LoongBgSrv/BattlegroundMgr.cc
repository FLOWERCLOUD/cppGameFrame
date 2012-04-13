/*
 * BattlegroundMgr.cc
 *
 *  Created on: 2012-3-22
 *    
 */

#include <game/LoongBgSrv/BattlegroundMgr.h>

#include <mysdk/base/Logging.h>

#include <game/LoongBgSrv/Battleground.h>

BattlegroundMgr::BattlegroundMgr(LoongBgSrv* srv):
	pSrv_(srv)
{
	init();
}

BattlegroundMgr::~BattlegroundMgr()
{

}

bool BattlegroundMgr::init()
{
	for (int i = 0; i < sMaxBgNum; i++)
	{
		bgList_[i].setSrv(pSrv_);
		bgList_[i].setId(i + 1);
	}
	return true;
}

void BattlegroundMgr::run(uint32 curTime)
{
	static int sStartIndex = 0;
	static const int sFragment = 3;

	for (int i = sStartIndex; i < sMaxBgNum ; )
	{
		if (bgList_[i].getState() != BattlegroundState::BGSTATE_NONE)
		{
			bgList_[i].run(curTime);
		}
		i += sFragment; //
	}

	sStartIndex++;
	sStartIndex = (sStartIndex >= sFragment) ? 0 : sStartIndex;
}

void BattlegroundMgr::shutdown()
{
	for (int i = 0; i < sMaxBgNum; i++)
	{
		bgList_[i].shtudown();
	}
}

Battleground& BattlegroundMgr::getBattleground(int16 bgId)
{
	return bgList_[bgId - 1];
}

bool BattlegroundMgr::checkBattlegroundId(int16 bgId) const
{
	if (bgId > 0 && bgId < sMaxBgNum) return true;
	return false;
}
