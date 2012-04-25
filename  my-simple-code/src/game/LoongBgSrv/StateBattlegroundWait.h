/*
 * StateBattlegroundWait.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_STATEBATTLEGROUNDWAIT_H_
#define GAME_STATEBATTLEGROUNDWAIT_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BattlegroundState.h>

class StateBattlegroundWait:public BattlegroundState
{
public:
	StateBattlegroundWait(Battleground* bg);
	virtual ~StateBattlegroundWait();

	virtual void run(int64 curTime);

	virtual BgStateE getState() { return BGSTATE_WAIT; }
	virtual uint32	getStateTimeLimit() { return 20000; } //55000; }
	virtual std::string getStateName() { return "准备阶段"; }
};

#endif /* STATEBATTLEGROUNDWAIT_H_ */
