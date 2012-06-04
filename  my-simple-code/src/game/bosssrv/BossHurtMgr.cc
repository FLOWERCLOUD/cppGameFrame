/*
 * BossHurtMgr.cc
 *
 *  Created on: 2012-5-30
 *    
 */

#include <game/bosssrv/BossHurtMgr.h>

#include <algorithm>
#include<iterator>

BossHurtMgr::BossHurtMgr(BossSrv* bossSrv):
	bossSrv_(bossSrv)
{

}

BossHurtMgr::~BossHurtMgr()
{
	size_t topNum = topPlayerVector_.size();
	for (size_t i = 0; i < topNum; i++)
	{
		Player* player = topPlayerVector_[i];
		if (player)
		{
			delete player;
		}
	}
}

void BossHurtMgr::addHurt(uint32 uid, uint32 hurtValue, char* username)
{
	uint32 oldHurtValue = getHurtValue(uid);
	uint32 newHurtValue = hurtValue + oldHurtValue;
	playerMap_[uid] = newHurtValue;

	addTop(uid, newHurtValue, username);
}

uint32 BossHurtMgr::getHurtValue(uint32 uid)
{
	PlayerHurtMapT::iterator iter;
	iter = playerMap_.find(uid);
	if (iter != playerMap_.end())
	{
		return iter->second;
	}
	return 0;
}

void BossHurtMgr::addTop(uint32 uid, uint32 hurtValue, char* username)
{
	size_t topNum = topPlayerVector_.size();
	// 看看他是否在排行榜中
	for (size_t i = 0; i < topNum; i++)
	{
		Player* player = topPlayerVector_[i];
		if (player)
		{
			if (player->uid == uid)
			{
				player->hurtvalue = hurtValue;

				sort(topPlayerVector_.begin(), topPlayerVector_.end());
				return;
			}
		}
	}

	Player* newPlayer = new Player;
	if (!newPlayer) return;

	newPlayer->uid = uid;
	newPlayer->hurtvalue = hurtValue;
	newPlayer->username = username;
	topPlayerVector_.push_back(newPlayer);

	sort(topPlayerVector_.begin(), topPlayerVector_.end());
	if (topNum == sMaxTopNum)
	{
		Player* tmp = topPlayerVector_[topNum];
		topPlayerVector_.pop_back();
		delete tmp;
	}
}

bool BossHurtMgr::serializeTop(PacketBase& op)
{
	size_t topNum = topPlayerVector_.size();
	op.putInt32(topNum);
	// 看看他是否在排行榜中
	for (size_t i = 0; i < topNum; i++)
	{
		Player* player = topPlayerVector_[i];
		if (player)
		{
			op.putInt32(player->uid);
			op.putUTF(player->username);
			op.putInt32(player->hurtvalue);
		}
	}
	return true;
}

