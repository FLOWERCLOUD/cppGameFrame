/*
 * Player.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/Player.h>

#include <game/LoongBgSrv/pet/Pet.h>
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <mysdk/net/TcpConnection.h>


Player::Player(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	playerId_(playerId),
	playerName_(name),
	x_(0),
	y_(0),
	team_(kNONE),
	killEnemyTimes_(0),
	pet_(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{

}

Player::Player(int32 playerId, std::string& name, TeamE team, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv):
	playerId_(playerId),
	playerName_(name),
	x_(0),
	y_(0),
	killEnemyTimes_(0),
	team_(team),
	pet_(NULL),
	pCon_(pCon),
	pSrv_(pSrv)
{
}

Player::~Player()
{

}

int32 Player::getPlayerId()
{
	return playerId_;
}

Pet* Player::getPet()
{
	return pet_;
}

void Player::addBuffer(Buf* buf)
{
	bufList_.push_back(buf);
}

void Player::update()
{

}

// 伤害
void Player::onHurt(Player* attacker, int16 damage, int16 skillId)
{
	if (!pet_)
	{
		pet_->decHp(hp);
	}
}

// 回血
void Player::onRiseHp(int16 hp)
{
	if (pet_)
	{
		pet_->addHp(hp);
	}
}

// 减血
void Player::onDecHp(int16 hp)
{
	if (!pet_)
	{
		pet_->decHp(hp);
	}
}

int16 Player::getX()
{
	return x_;
}

int16 Player::getY()
{
	return y_;
}

int16 Player::getKillEnemyTimes()
{
	return killEnemyTimes_;
}

void Player::incKillEnemyTimes()
{
	killEnemyTimes_++;
}

std::string Player::getPlayerName()
{
	return playerName_;
}

void Player::sendPacket(PacketBase& op)
{
	if (pSrv_ && pCon_)
	{
		LOG_TRACE << " sendpacket ";
		pSrv_->send(pCon_, op);
	}
}
