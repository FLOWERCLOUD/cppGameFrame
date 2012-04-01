/*
 * ItemBaseMgr.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_ITEMBASEMGR_H_
#define GAME_ITEMBASEMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <game/LoongBgSrv/base/ItemBase.h>

class ItemBaseMgr
{
public:
	static const int sMaxItemId = 10;
public:
	ItemBaseMgr();
	~ItemBaseMgr();

	bool init();
	void shutdown();

	bool checkItemId(int16 itemId);
	const ItemBase& getItemBaseInfo(int16 itemId) const;
private:
	int16 curItemBaseNum_;
	ItemBase itemBaseList_[sMaxItemId ];
private:
	DISALLOW_COPY_AND_ASSIGN(ItemBaseMgr);
};

#define sItemBaseMgr mysdk::Singleton<ItemBaseMgr>::instance()

#endif /* ITEMBASEMGR_H_ */
