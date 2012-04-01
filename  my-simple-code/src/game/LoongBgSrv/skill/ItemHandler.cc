/*
 * ItemHandler.cc
 *
 *  Created on: 2012-3-29
 *    
 */

#include <game/LoongBgSrv/skill/ItemHandler.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <game/LoongBgSrv/base/ItemBaseMgr.h>
#include <game/LoongBgSrv/skill/BufHandler.h>
#include <game/LoongBgSrv/BgUnit.h>

typedef bool (*itemFunction)(int16 itemId, BgUnit* me, BgUnit* target, Scene* scene);

static bool defaultItemFunction(int16 itemId, BgUnit* me, BgUnit* target, Scene* scene)
{
	return true;
}

static bool addHpItemFunction(int16 itemId, BgUnit* me, BgUnit* target, Scene* scene)
{
	me->fullHp();
	return true;
}

static itemFunction ItemFunction[] =
{
		defaultItemFunction,
		addHpItemFunction,
};

bool ItemHandler::onUseItem(int16 itemId, BgUnit* me, BgUnit* target, Scene* scene)
{
	if (!sItemBaseMgr.checkItemId(itemId))
	{
		return false;
	}

	const ItemBase& itemBase = sItemBaseMgr.getItemBaseInfo(itemId);
	int16 functionType = itemBase.functionType_;
	assert(functionType < static_cast<int16>(sizeof(ItemFunction) / sizeof(itemFunction)));
	ItemFunction[functionType](itemId,me, target, scene);

	// buff 处理哦
	int16 bufNum = itemBase.bufNum_;
	for (int16 i = 0; i < bufNum; i++)
	{
		int16 bufId = itemBase.bufList_[i];
		const BufBase& bufbase = sBufBaseMgr.getBufBaseInfo(bufId);
		int16 bufType = bufbase.type_;
		BufHandler::bufHandlers[bufType](bufId, 0, me, target, scene);
	}

	return true;
}

