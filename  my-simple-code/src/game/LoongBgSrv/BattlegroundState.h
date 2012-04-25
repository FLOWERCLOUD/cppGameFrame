/*
 * BattlegroundState.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_BATTLEGROUNDSTATE_H_
#define GAME_BATTLEGROUNDSTATE_H_

#include <mysdk/base/Common.h>

#include <string>

using namespace mysdk;
class Battleground;
class BattlegroundState
{
public:
	typedef enum 	tagBgStateE
	{
		BGSTATE_NONE					= 0,
		BGSTATE_WAIT					= 1,
		BGSTATE_COUNTDOWN	=	2,
		BGSTATE_START					= 3,
		BGSTATE_RUN						=	4,
		BGSTATE_EXIT						=	5,

		BGSTATE_COUNT					= 6,
	}BgStateE;
public:
	BattlegroundState(Battleground* bg);
	virtual ~BattlegroundState();

	virtual void start();
	virtual void run(int64 curTime);
	virtual void end();

	virtual BgStateE getState() { return BGSTATE_NONE; }
	virtual uint32	getStateTimeLimit() { return 99999999; }
	virtual std::string getStateName() { return "null!!!"; }

	uint32 getLeftTime();
protected:
	Battleground* pBattleground_;
	int64 startTime_;
};

#endif /* BATTLEGROUNDSTATE_H_ */
