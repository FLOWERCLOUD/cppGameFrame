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
	flowerMgr_(this),
	blackBuildings_(1, KFOUR_UNITTYPE, BgUnit::kBlack_TEAM,  this),
	whiteBuildings_(2, KFOUR_UNITTYPE, BgUnit::kWhite_TEAM, this)
{
	blackBuildings_.setX(2773);
	blackBuildings_.setY(258);

	whiteBuildings_.setX(129);
	whiteBuildings_.setY(209);
}

Scene::~Scene()
{
}

bool Scene::addPlayer(BgPlayer* player)
{
	assert(player != NULL);

	int32 playerId = player->getId();
	playerMgr_[playerId] = player;
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
	//playerMgr_[playerId] = NULL;
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
	blackBuildings_.init();
	whiteBuildings_.init();

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

void Scene::run(int64 curTime)
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

BgFlower* Scene::getFlower(int32 flowerId)
{
	return flowerMgr_.getFlower(flowerId);
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
	op.putInt32(blackBuildings_.getHp());
	op.putInt32(whiteBuildings_.getHp());
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

