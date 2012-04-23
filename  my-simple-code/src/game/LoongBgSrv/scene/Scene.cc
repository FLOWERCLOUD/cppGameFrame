/*
 * Scene.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/scene/Scene.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/BgPlayer.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

Scene::Scene():
	sceneId_(0),
	dropItemMgr_(this),
	flowerMgr_(this)
{

}

Scene::~Scene()
{
}

bool Scene::addPlayer(BgPlayer* player)
{
	assert(player != NULL);
	int32 playerId = player->getId();
	playerMgr_.insert(std::pair<int32, BgPlayer*>(playerId, player));
	player->setScene(this);

	PacketBase op(client::OP_ADD_PLAYER, 0);
	player->serialize(op);
	broadMsg(op);
	return true;
}

bool Scene::removePlayer(BgPlayer* player)
{
	assert(player != NULL);

	int32 playerId = player->getId();
	playerMgr_.erase(playerId);

	PacketBase op(client::OP_REMOVE_PLAYER, playerId);
	broadMsg(op);
	return true;
}

void Scene::broadMsg(PacketBase& op)
{
	std::map<int32, BgPlayer*>::iterator iter;
	for(iter = playerMgr_.begin(); iter != playerMgr_.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			player->sendPacket(op);
		}
	}
}

bool Scene::init()
{
	dropItemMgr_.init();
	playerMgr_.clear();
	flowerMgr_.init();
	return true;
}

void Scene::shutdown()
{
	dropItemMgr_.shutdown();
	flowerMgr_.shutdown();
}

void Scene::run(uint32 curTime)
{
	std::map<int32, BgPlayer*>::iterator iter;
	for(iter = playerMgr_.begin(); iter != playerMgr_.end(); iter++)
	{
		BgPlayer* player = iter->second;
		if (player)
		{
			player->run(curTime);
		}
	}
	dropItemMgr_.run(curTime);
	flowerMgr_.run(curTime);
}

BgPlayer* Scene::getPlayer(int playerId)
{
	std::map<int32, BgPlayer*>::iterator iter;
	iter = playerMgr_.find(playerId);
	if (iter != playerMgr_.end())
	{
		return iter->second;
	}
	return NULL;
}

bool Scene::hasItem(int16 x, int16 y)
{
	return false;
}

bool Scene::pickUpItem(BgPlayer* player, int16 x, int16 y)
{
	return dropItemMgr_.pickUpItem(player, x, y);
}

bool Scene::serialize(PacketBase& op)
{
	serializeItem(op);
	serializeFlower(op);
	return true;
}

bool Scene::serializeItem(PacketBase& op)
{
	return dropItemMgr_.serialize(op);
}

bool Scene::serializePlayer(PacketBase& op)
{
	return true;
}

bool Scene::plantFlower(BgPlayer* player, int16 x, int16 y)
{
	return flowerMgr_.plantFlower(player, x, y);
}

bool Scene::serializeFlower(PacketBase& op)
{
	return flowerMgr_.serialize(op);
}
