/*
 * Item.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/package/Item.h>

Item::Item():
	itemId_(0),
	num_(0)
{

}

Item::Item(int16 itemId, int16 num):
	itemId_(itemId),
	num_(num)
{
}

Item::~Item()
{
}

int16 Item::getItemId()
{
	return itemId_;
}

int16 Item::getNum()
{
	return num_;
}

void Item::setItemId(int16 itemId)
{
	itemId_ = itemId;
}

void Item::setItemNum(int16 num)
{
	num_ = num;
}

