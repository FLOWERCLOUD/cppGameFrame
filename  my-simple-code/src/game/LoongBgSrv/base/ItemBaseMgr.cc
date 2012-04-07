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
	//init();
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

bool ItemBaseMgr::init(TestDatabaseWorkerPool& databaseWorkPool)
{
	const char* sql = "select itemid, itemname, functionType, itemBuff from item";
	ResultSet* res = databaseWorkPool.query(sql);
	if (!res) return false;

	while (res->nextRow())
	{
		const Field* field = res->fetch();
		itemBaseList_[curItemBaseNum_].itemId_ =  field[0].getInt16();
		itemBaseList_[curItemBaseNum_].itemName_ = field[1].getString();
		itemBaseList_[curItemBaseNum_].functionType_ =  field[2].getInt16();
		ColonBuf buf(field[3].getCString());
		int16 num = itemBaseList_[curItemBaseNum_].bufNum_= buf.GetShort();
		for (int16 i = 0; i < num; i++)
		{
			itemBaseList_[curItemBaseNum_].bufList_[i] = buf.GetShort();
		}
		curItemBaseNum_++;
	}
	delete res;

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

