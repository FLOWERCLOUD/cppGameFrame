/*
 * FlowerMgr.cc
 *
 *  Created on: 2012-4-23
 *    
 */

#include <game/LoongBgSrv/scene/FlowerMgr.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/Util.h>

FlowerMgr::FlowerMgr(Scene* pScene)
{
	init();
}

FlowerMgr::~FlowerMgr()
{

}

bool FlowerMgr::init()
{
	flowerList_.clear();
	return true;
}

void FlowerMgr::run(uint32 curTime)
{
	std::list<struct Flower>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); )
	{
		uint32 plantTime = (*itList).plantTimes;
		if (curTime - plantTime > sDisappearTime)
		{
			// 告诉客户端知道 这个食人花要消失啦
			PacketBase op(client::OP_DISAPPEAR_FLOWER, 0);
			op.putInt32((*itList).x);
			op.putInt32((*itList).y);

			itList = flowerList_.erase(itList);
		}
		else
		{
			++itList;
		}
	}
}

void FlowerMgr::shutdown()
{

}

bool FlowerMgr::plantFlower(BgPlayer* player, int16 x, int16 y)
{
	assert(pScene_);

	int16 itemId = 4; //食人花种子id
	if (!player->hasItem(4)) //
	{
		return false;
	}

	if (flowerList_.size() > sMaxIFlowerNum)
	{
		// 告诉客户端 战场不能种植食人花啦
		PacketBase op(client::OP_PLANT_FLOWER, -1);
		player->sendPacket(op);
		return false;
	}

	struct Flower flower;
	flower.x = x;
	flower.y = y;
	flower.plantTimes = getCurTime();
	flowerList_.push_back(flower);
	player->delItem(itemId);

	PacketBase op(client::OP_PLANT_FLOWER, 0);
	op.putInt32(x);
	op.putInt32(y);
	pScene_->broadMsg(op);
	return true;
}

bool FlowerMgr::serialize(PacketBase& op)
{
	int32 itemNum = static_cast<int32>(flowerList_.size());
	op.putInt32(itemNum);
	std::list<struct Flower>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); ++itList)
	{
		op.putInt32((*itList).x);
		op.putInt32((*itList).y);
	}
	return true;
}
