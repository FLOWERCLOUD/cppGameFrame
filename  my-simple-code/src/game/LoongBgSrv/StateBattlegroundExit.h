/*
 * StateBattlegroundExit.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_STATEBATTLEGROUNDEXIT_H_
#define GAME_STATEBATTLEGROUNDEXIT_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BattlegroundState.h>

class StateBattlegroundExit:public BattlegroundState
{
public:
	StateBattlegroundExit(Battleground* bg);
	virtual ~StateBattlegroundExit();

	virtual void run(uint32 curTime);
	virtual void end();

	virtual BgStateE getState() { return BGSTATE_EXIT; }
	virtual uint32	getStateTimeLimit() { return 10; }
	virtual std::string getStateName() { return "关闭阶段"; }
};

#endif /* STATEBATTLEGROUNDEXIT_H_ */
