/*
 * package.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PACKAGE_H_
#define GAME_PACKAGE_H_

#include <mysdk/base/Common.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <game/LoongBgSrv/package/Item.h>

class Package
{
public:
	static const int sMaxItemNum = 3;
public:
	Package();
	~Package();

	bool hasItem(int16 itemId);
	// 一次只能使用一个物品
	void delItem(int16 itemId);
	typedef enum tagAddItemRtnE
	{
		kNone_AddItemRtn			= 0,
		kSuccess_AddItemRtn		= 	1, //添加物品成功
		kPackageFull_AddItemRtn	= 2, //背包满了
		kError_AddItemRtn				= 3, // 添加物品失败

	} AddItemRtnE;
	// 一次只能添加一个物品
	AddItemRtnE addItem(int16 itemId);

	void clear();

	bool serialize(PacketBase& op);
	bool isFull();
private:
	int16 curItemNum_;
	Item itemList_[sMaxItemNum];
private:
	DISALLOW_COPY_AND_ASSIGN(Package);
};

#endif /* PACKAGE_H_ */
