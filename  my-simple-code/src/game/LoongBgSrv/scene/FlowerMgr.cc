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

FlowerMgr::FlowerMgr(Scene* pScene):
	pScene_(pScene)
{
	init();
}

FlowerMgr::~FlowerMgr()
{

}

static uint32 sStartId ; //食人花的唯一id

bool FlowerMgr::init()
{
	sStartId = 1;
	std::list<BgFlower*>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); ++itList)
	{
		BgFlower* flower = *itList;
		delete flower;
	}
	flowerList_.clear();
	return true;
}

void FlowerMgr::run(uint32 curTime)
{
	std::list<BgFlower*>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); )
	{
		BgFlower* flower = *itList;
		uint32 plantTime = flower->getPlantTimes();
		if (curTime - plantTime > sDisappearTime || flower->isDead())
		{
			// 告诉客户端知道 这个食人花要消失啦
			PacketBase op(client::OP_DISAPPEAR_FLOWER, 0);
			flower->serialize(op);
			pScene_->broadMsg(op);
			itList = flowerList_.erase(itList);

			delete flower;
		}
		else
		{
			flower->run(curTime);
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

	BgFlower* flower = new BgFlower(sStartId++, KFIFTH_UNITTYPE, player->getTeam(), pScene_);
	if (!flower) return false;

	flower->setX(x);
	flower->setY(y);
	flower->setPlantTimes(getCurTime());
	flowerList_.push_back(flower);
	player->delItem(itemId);

	PacketBase op(client::OP_PLANT_FLOWER, 0);
	flower->serialize(op);
	pScene_->broadMsg(op);
	return true;
}

BgFlower* FlowerMgr::getFlower(int32 flowerId)
{
	std::list<BgFlower*>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); ++itList)
	{
		BgFlower* flower = *itList;
		if (flower->getId() == flowerId)
		{
			return flower;
		}
	}
	return NULL;
}

bool FlowerMgr::serialize(PacketBase& op)
{
	int32 itemNum = static_cast<int32>(flowerList_.size());
	op.putInt32(itemNum);
	std::list<BgFlower*>::iterator itList;
	for (itList = flowerList_.begin(); itList != flowerList_.end(); ++itList)
	{
		BgFlower* flower = *itList;
		if (flower)
		{
			flower->serialize(op);
		}
	}
	return true;
}
