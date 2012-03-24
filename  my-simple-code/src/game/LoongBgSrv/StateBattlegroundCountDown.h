/*
 * StateBattlegroundCountDown.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_STATEBATTLEGROUNDCOUNTDOWN_H_
#define GAME_STATEBATTLEGROUNDCOUNTDOWN_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BattlegroundState.h>

class StateBattlegroundCountDown:public BattlegroundState
{
public:
	StateBattlegroundCountDown(Battleground* bg);
	virtual ~StateBattlegroundCountDown();

	virtual void run(uint32 curTime);

	virtual BgStateE getState() { return BGSTATE_COUNTDOWN; }
	virtual uint32	getStateTimeLimit() { return 5; }
	virtual std::string getStateName() { return "倒计时阶段"; }
};

#endif /* STATEBATTLEGROUNDCOUNTDOWN_H_ */
