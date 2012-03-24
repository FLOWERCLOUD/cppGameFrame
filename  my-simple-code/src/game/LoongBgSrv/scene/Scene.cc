/*
 * Scene.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/scene/Scene.h>

#include <game/LoongBgSrv/BgPlayer.h>

Scene::Scene():
	sceneId_(0)
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
	return true;
}

bool Scene::removePlayer(BgPlayer* player)
{
	assert(player != NULL);
	int32 playerId = player->getId();
	playerMgr_.erase(playerId);
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
}

BgPlayer* Scene::getPlayer(int playerId)
{
	return NULL;
}
