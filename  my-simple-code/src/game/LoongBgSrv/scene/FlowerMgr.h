/*
 * FlowerMgr.h
 *
 *  Created on: 2012-4-23
 *    
 */

#ifndef GAME_FLOWERMGR_H_
#define GAME_FLOWERMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <game/LoongBgSrv/BgFlower.h>

#include <list>

using namespace mysdk;

class Scene;
class BgPlayer;
class FlowerMgr
{
public:
	static const uint32 sMaxIFlowerNum = 5; //最大食人花数量
	static const uint32 sDisappearTime = 30000; //30s
public:
	FlowerMgr(Scene* pScene);
	~FlowerMgr();

	bool init();
	void run(uint32 curTime);
	void shutdown();

	bool plantFlower(BgPlayer* player, int16 x, int16 y);
	bool serialize(PacketBase& op);

	BgFlower* getFlower(int32 flowerId);
private:
	Scene* pScene_;
	std::list<BgFlower*> flowerList_;
private:
	DISALLOW_COPY_AND_ASSIGN(FlowerMgr);
};

#endif /* FLOWERMGR_H_ */
