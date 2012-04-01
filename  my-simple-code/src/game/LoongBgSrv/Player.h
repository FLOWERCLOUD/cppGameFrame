/*
 * Player.h
 *
 *  Created on: 2012-3-31
 *    
 */

#ifndef GAME_PLAYER_H_
#define GAME_PLAYER_H_

#include <mysdk/base/Common.h>

using namespace mysdk;

class BgPlayer;
class Player
{
public:
	Player();
	~Player();

	bool isOnline();

	BgPlayer* player_;
};

#endif /* PLAYER_H_ */
