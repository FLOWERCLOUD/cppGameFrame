/*
 * PlayerMgr.h
 *
 *  Created on: 2012-3-31
 *    
 */

#ifndef GAME_PLAYERMGR_H_
#define GAME_PLAYERMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>
#include <map>

using namespace mysdk;
class Player;
class PlayerMgr
{
public:
	typedef std::map<int32, Player*> PlayerMgrMapT;
public:
	PlayerMgr();
	~PlayerMgr();

	bool isOnline(int32 playerId);
	bool addPlayer(int32 playerId, Player* player);
	Player* getPlayer(int32 playerId);
private:
	PlayerMgrMapT playerMap_;
private:
	DISALLOW_COPY_AND_ASSIGN(PlayerMgr);
};

#define sPlayerMgr mysdk::Singleton<PlayerMgr>::instance()

#endif /* PLAYERMGR_H_ */
