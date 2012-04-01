/*
 * Player.cc
 *
 *  Created on: 2012-3-31
 *    
 */

#include <game/LoongBgSrv/Player.h>
#include <game/LoongBgSrv/Util.h>

Player::Player():
	player_(NULL)
{

}

Player::~Player()
{
}

bool Player::isOnline()
{
	return player_ == NULL ? false : true;
}
