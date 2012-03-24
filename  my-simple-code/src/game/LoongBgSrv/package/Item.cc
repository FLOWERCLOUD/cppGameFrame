/*
 * Item.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgsrv/package/Item.h>

Item::Item(int16 itemId, int16 num, int16 type):
	itemId_(itemId),
	num_(num),
	type_(type)
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

int16 Item::getType()
{
	return type_;
}

