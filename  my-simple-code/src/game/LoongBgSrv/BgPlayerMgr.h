/*
 * BgPlayerMgr.h
 *
 *  Created on: 2012-4-1
 *    
 */

#ifndef GAME_BGPLAYERMGR_H_
#define GAME_BGPLAYERMGR_H_

#include <mysdk/base/Common.h>

#include <map>

class BgPlayer;
class BgPlayerMgr
{
public:
	typedef std::map<int32, BgPlayer*> BgPlayerMapT;

public:
	BgPlayerMgr();
	~BgPlayerMgr();

	bool hasBgPlayer(int32 playerId);
	void addBgPlayer(int32 playerId, BgPlayer* player);
	void removeBgPlayer(int32 playerId);
private:
	BgPlayerMapT bgPlayerMap_;
private:
	DISALLOW_COPY_AND_ASSIGN(BgPlayerMgr);
};
#endif /* BGPLAYERMGR_H_ */
