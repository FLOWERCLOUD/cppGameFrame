/*
 * BgClientDataMgr.h
 *
 *  Created on: 2012-4-19
 *    
 */

#ifndef GAME_BGCLIENTDATAMGR_H_
#define GAME_BGCLIENTDATAMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <map>

using namespace mysdk;
class BgClientData;
class BgClientDataMgr
{
public:
	typedef std::map<int32, BgClientData*> BgClientDataMapT;
public:
	BgClientDataMgr();
	~BgClientDataMgr();

	void addPlayer(BgClientData* bgClient);
	void removePlayer(int32 playerId);
	BgClientData* getPlayer(int32 playerId);
	void printInfo();

	BgClientDataMapT& getClientMap()
	{
		return bgClientDataMap_;
	}
private:
	BgClientDataMapT bgClientDataMap_;

private:
};

#define sBgClientDataMgr mysdk::Singleton<BgClientDataMgr>::instance()

#endif /* BGCLIENTDATAMGR_H_ */
