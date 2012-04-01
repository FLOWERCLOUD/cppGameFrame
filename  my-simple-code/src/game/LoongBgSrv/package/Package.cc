/*
 * package.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/package/Package.h>

Package::Package():
	curItemNum_(0)
{

}

Package::~Package()
{

}

bool Package::hasItem(int16 itemId)
{
	for (int16 i = 0; i < sMaxItemNum; i++)
	{
		if (itemList_[i].getItemId() == itemId)
		{
			return true;
		}
	}
	return false;
}

// 一次只能使用一个物品
void Package::delItem(int16 itemId)
{
	for (int16 i = 0; i < sMaxItemNum ; i++)
	{
		if (itemList_[i].getItemId() == itemId)
		{
			itemList_[i].setItemId(0);
			itemList_[i].setItemNum(0);
			curItemNum_--;
			return;
		}
	}
	return;
}

// 一次只能添加一个物品
Package::AddItemRtnE Package::addItem(int16 itemId)
{
	if (curItemNum_ >= sMaxItemNum )
	{
		return kPackageFull_AddItemRtn;
	}

	for (int16 i = 0; i < sMaxItemNum; i++)
	{
		if (itemList_[i].getItemId() == 0)
		{
			curItemNum_++;
			itemList_[i].setItemId(itemId);
			itemList_[i].setItemNum(1);
			return kSuccess_AddItemRtn;
		}
	}
	return kError_AddItemRtn;
}

bool Package::serialize(PacketBase& op)
{
	op.putInt32(curItemNum_);
	for (int16 i = 0; i < sMaxItemNum; i++)
	{
		int16 itemId = itemList_[i].getItemId();
		if (itemId > 0)
		{
			op.putInt32(itemId);
		}
	}
	return true;
}
