/*
 * BattlegroundMgr.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_BATTLEGROUNDMGR_H_
#define GAME_BATTLEGROUNDMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <game/LoongBgSrv/Battleground.h>

using namespace mysdk;
class BattlegroundMgr
{
public:
	static const int16 sMaxBgNum = 500;
public:
	BattlegroundMgr();
	~BattlegroundMgr();

	bool init();
	void run(uint32 curTime);
	void shutdown();

	bool checkBattlegroundId(int16 bgId);
	Battleground& getBattleground(int16 bgId);
private:
	Battleground bgList_[sMaxBgNum];
private:
	DISALLOW_COPY_AND_ASSIGN(BattlegroundMgr);
};

#define sBattlegroundMgr mysdk::Singleton<BattlegroundMgr>::instance()

#endif /* BATTLEGROUNDMGR_H_ */
