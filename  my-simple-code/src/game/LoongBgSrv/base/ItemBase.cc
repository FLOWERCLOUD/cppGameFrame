/*
 * ItemBase.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/base/ItemBase.h>

ItemBase::ItemBase():
	itemId_(0),
	itemName_("item"),
	functionType_(0),
	bufNum_(0)
{
	for (int i = 0; i < sMaxBufNum; i++)
	{
		bufList_[i] = 0;
	}
}

ItemBase::~ItemBase()
{

}
