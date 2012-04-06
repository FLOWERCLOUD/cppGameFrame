/*
 * ItemBaseMgr.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/base/ItemBaseMgr.h>

ItemBaseMgr::ItemBaseMgr():
	curItemBaseNum_(0)
{
	init();
}

ItemBaseMgr::~ItemBaseMgr()
{

}

bool ItemBaseMgr::init()
{
	itemBaseList_[curItemBaseNum_].itemId_ = 1;
	itemBaseList_[curItemBaseNum_].itemName_ = "生命药水";
	itemBaseList_[curItemBaseNum_].functionType_  = 1;
	curItemBaseNum_++;
	itemBaseList_[curItemBaseNum_].itemId_ = 2;
	itemBaseList_[curItemBaseNum_].itemName_ = "无敌药水";
	itemBaseList_[curItemBaseNum_].functionType_  = 0;
	itemBaseList_[curItemBaseNum_].bufNum_ = 1;
	itemBaseList_[curItemBaseNum_].bufList_[0] = 4;

	curItemBaseNum_++;
	itemBaseList_[curItemBaseNum_].itemId_ = 3;
	itemBaseList_[curItemBaseNum_].itemName_ = "腾云靴";
	itemBaseList_[curItemBaseNum_].functionType_  = 0;
	curItemBaseNum_++;
	itemBaseList_[curItemBaseNum_].itemId_ = 4;
	itemBaseList_[curItemBaseNum_].itemName_ = "食人花种子";
	itemBaseList_[curItemBaseNum_].functionType_  = 0;

	return true;
}

void ItemBaseMgr::shutdown()
{

}


bool ItemBaseMgr::checkItemId(int16 itemId)
{
	if (itemId > 0 && itemId <= curItemBaseNum_) return true;

	return false;
}

const ItemBase& ItemBaseMgr::getItemBaseInfo(int16 itemId) const
{
	static ItemBase null;
	if (itemId <= 0 || itemId > sMaxItemId) return null;

	return itemBaseList_[itemId - 1];
}

