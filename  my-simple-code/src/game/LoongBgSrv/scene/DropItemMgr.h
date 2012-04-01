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

class Scene;
class BgPlayer;
class DropItemMgr
{
public:
	static const uint32 sTenseconds = 10000; //10s 单位是ms
	static const uint32 sMaxItemNum = 20; //
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
	std::map<int32, int32> itemList_;

	uint32 lastTime_;
	Scene* pScene_;
private:
	DISALLOW_COPY_AND_ASSIGN(DropItemMgr);
};

#endif /* DROPITEMMGR_H_ */
