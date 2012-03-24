/*
 * StateBattlegroundStart.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_STATEBATTLEGROUNDSTART_H_
#define GAME_STATEBATTLEGROUNDSTART_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BattlegroundState.h>

class StateBattlegroundStart:public BattlegroundState
{
public:
	StateBattlegroundStart(Battleground* bg);
	virtual ~StateBattlegroundStart();

	virtual void run(uint32 curTime);

	virtual BgStateE getState() { return BGSTATE_WAIT; }
	virtual uint32	getStateTimeLimit() { return 10; }
	virtual std::string getStateName() { return "开始阶段"; }
};

#endif /* STATEBATTLEGROUNDSTART_H_ */
