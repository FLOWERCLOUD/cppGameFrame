/*
 * BattlegroundMgr.cc
 *
 *  Created on: 2012-3-22
 *    
 */

#include <game/LoongBgSrv/BattlegroundMgr.h>

#include <game/LoongBgSrv/Battleground.h>

BattlegroundMgr::BattlegroundMgr()
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
		bgList_[i].setId(i + 1);
	}
	return true;
}

void BattlegroundMgr::run(uint32 curTime)
{
	for (int i = 0; i < sMaxBgNum; i++)
	{
		if (bgList_[i].getState() != BattlegroundState::BGSTATE_NONE)
		{
			bgList_[i].run(curTime);
		}
	}
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

bool BattlegroundMgr::checkBattlegroundId(int16 bgId)
{
	if (bgId > 0 && bgId < sMaxBgNum) return true;
	return false;
}
