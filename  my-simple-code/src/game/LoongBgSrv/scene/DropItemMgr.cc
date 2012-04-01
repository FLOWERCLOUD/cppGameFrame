/*
 * DropItemMgr.cc
 *
 *  Created on: 2012-3-31
 *    
 */

#include <game/LoongBgSrv/scene/DropItemMgr.h>
#include <game/LoongBgSrv/scene/Scene.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/Util.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>

DropItemMgr::DropItemMgr(Scene* pScene):
	lastTime_(getCurTime()),
	pScene_(pScene)
{
}

DropItemMgr::~DropItemMgr()
{
}


bool DropItemMgr::init()
{
	return true;
}

void DropItemMgr::run(uint32 curTime)
{
	if (curTime - lastTime_ > sTenseconds)
	{
		if (itemList_.size() > sMaxItemNum) return;

		size_t pointNum = pointList_.size();
		if (pointNum == 0) return;

		int32  pointIndex = getRandomBetween(0, pointNum);
		int16 x = pointList_[pointIndex].x;
		int16 y = pointList_[pointIndex].y;
		int32  pointKey = MakeInt32(x, y);
		// 移除这个坐标点
		int32 lastIndex = pointNum - 1;
		if (pointIndex != lastIndex)
		{
			pointList_[pointIndex].x = pointList_[lastIndex].x;
			pointList_[pointIndex].y = pointList_[lastIndex].y;
		}
		pointList_.pop_back();


		// 生成一个物品
		// 生命药水
		// 无敌药水
		// 腾云靴
		// 食人花种子
		static const int32 itemList[] = {1, 2, 3, 4};
		int32 itemIdIndex = getRandomBetween(0, sizeof(itemList) / sizeof(int32));
		int32 itemId = itemList[itemIdIndex];
		itemList_.insert(std::pair<int32, int32>(pointKey, itemList[itemIdIndex]));

		lastTime_ = curTime;

		// 广播给玩家知道, 随机生成了一个物品啦
		PacketBase op(client::OP_DROP_ITEM, 0);
		op.putInt32(itemId);
		op.putInt32(x);
		op.putInt32(y);
		pScene_->broadMsg(op);
	}
}

void DropItemMgr::shutdown()
{

}

bool DropItemMgr::pickUpItem(BgPlayer* player, int16 x, int16 y)
{
	int32  pointKey = MakeInt32(x, y);
	std::map<int32, int32>::iterator iter;
	iter = itemList_.find(pointKey);
	if (iter != itemList_.end())
	{
		int32 itemId = iter->second;
		if (player->addItem(itemId))
		{
			itemList_.erase(pointKey);
			struct point p;
			p.x = x;
			p.y = y;
			pointList_.push_back(p);
			// 告诉客户端 这个坐标点的物品给人拾取啦
			int32 playerId = player->getId();
			PacketBase op(client::OP_PICKUP_ITEM, 0);
			op.putInt32(playerId);
			op.putInt32(itemId);
			op.putInt32(x);
			op.putInt32(y);
			pScene_->broadMsg(op);
		}
		// 把这个物品添加到玩家的背包中
		return true;
	}
	else
	{
		//告诉玩家 这个坐标点 没有这个物品哦
		PacketBase op(client::OP_PICKUP_ITEM, -1);
		player->sendPacket(op);
	}

	return false;
}


bool DropItemMgr::serialize(PacketBase& op)
{
	size_t itemNum = itemList_.size();
	op.putInt32(itemNum);
	std::map<int32, int32>::iterator iter;
	for(iter = itemList_.begin(); iter != itemList_.end(); iter++)
	{
		int32 itemId = iter->second;
		op.putInt32(itemId);
	}
	return true;
}
