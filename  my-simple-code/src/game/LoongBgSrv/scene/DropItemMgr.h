/*
 * DropItemMgr.h
 *
 *  Created on: 2012-3-31
 *    
 */

#ifndef GAME_DROPITEMMGR_H_
#define GAME_DROPITEMMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <vector>
#include <map>

using namespace mysdk;
struct point
{
	int16 x;
	int16 y;
};

struct DropItem
{
	struct point point;
	int32 itemId;
};

class Scene;
class BgPlayer;
class DropItemMgr
{
public:
	static const uint32 sTenseconds = 30000; //30s 单位是ms
	static const uint32 sMaxItemNum = 20; // 一场战场中 最多有20个
	static const uint32 sPerItemNum = 5; // 每个物品最多5个
	static const uint32 sItemNum = 4; //现在只有4种物品哦
	static const uint32 sRandomItemNum = sPerItemNum * sItemNum;
public:
	DropItemMgr(Scene* pScene);
	~DropItemMgr();

	bool init();
	void run(uint32 curTime);
	void shutdown();
	// 拾取物品
	bool pickUpItem(BgPlayer* player, int16 x, int16 y);
	bool serialize(PacketBase& op);
private:
	std::vector<struct point> pointList_;
	std::map<int32, struct DropItem> itemList_;
	std::vector<int32> randomItemList_;

	uint32 lastTime_;
	Scene* pScene_;
private:
	DISALLOW_COPY_AND_ASSIGN(DropItemMgr);
};

#endif /* DROPITEMMGR_H_ */
