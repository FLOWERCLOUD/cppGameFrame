/*
 * Scene.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_SCENE_H_
#define GAME_SCENE_H_

#include <mysdk/base/Common.h>
#include <mysdk/protocol/kabu/codec/PacketBase.h>
#include <game/LoongBgSrv/scene/DropItemMgr.h>
#include <game/LoongBgSrv/scene/FlowerMgr.h>
#include <game/LoongBgSrv/BgBuilding.h>

#include <map>

class BgPlayer;
class BgFlower;
class Scene
{
public:
	Scene();
	~Scene();

	bool addPlayer(BgPlayer* player);
	bool removePlayer(BgPlayer* player);

	void broadMsg(PacketBase& op);

	bool init();
	void run(uint32 curTime);
	void shutdown();

	std::map<int32, BgPlayer*>& getPlayerMgr()
	{
		return playerMgr_;
	}
	BgPlayer* getPlayer(int playerId);
	BgFlower* getFlower(int32 flowerId);

	bool hasItem(int16 x, int16 y);
	bool pickUpItem(BgPlayer* player, int16 x, int16 y);
	bool plantFlower(BgPlayer* player, int16 x, int16 y);

	bool serialize(PacketBase& op);
	bool serializeItem(PacketBase& op);
	bool serializePlayer(PacketBase& op);
	bool serializeFlower(PacketBase& op);

	BgBuilding& getBlackBuilding()
	{
		return blackBuildings_;
	}

	BgBuilding& getWhiteBuilding()
	{
		return whiteBuildings_;
	}
private:
	int32 sceneId_;
	std::map<int32, BgPlayer*> playerMgr_;
	DropItemMgr dropItemMgr_;
	FlowerMgr flowerMgr_;

	BgBuilding blackBuildings_; //暗黑军王座
	BgBuilding whiteBuildings_;// 烈阳军王座
private:
	DISALLOW_COPY_AND_ASSIGN(Scene);
};
#endif /* SCENE_H_ */
