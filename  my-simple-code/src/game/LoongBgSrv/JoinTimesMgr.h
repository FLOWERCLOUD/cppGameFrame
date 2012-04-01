/*
 * JoinTimesMgr.h
 *
 *  Created on: 2012-4-1
 *    
 */

#ifndef GAME_JOINTIMESMGR_H_
#define GAME_JOINTIMESMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <map>
using namespace mysdk;
class JoinTimesMgr
{
public:
	typedef std::map<int32, int32> PlayerMgrT;
public:
	JoinTimesMgr();
	~JoinTimesMgr();

	bool init();
	void run(uint32 curTime);
	void shutdown();

	void incJoinTimes(int32 playerId);
	int32 getJoinTimes(int32 playerId);
private:
	PlayerMgrT playerMap_;
private:
	DISALLOW_COPY_AND_ASSIGN(JoinTimesMgr);
};

#define sJoinTimesMgr mysdk::Singleton<JoinTimesMgr>::instance()

#endif /* JOINTIMESMGR_H_ */
