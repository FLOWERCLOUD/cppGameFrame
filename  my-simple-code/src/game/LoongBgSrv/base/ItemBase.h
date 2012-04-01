/*
 * ItemBase.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_ITEMBASE_H_
#define GAME_ITEMBASE_H_

#include <mysdk/base/Common.h>

#include <string>
using namespace mysdk;
class ItemBase
{
public:
	static const int sMaxBufNum = 2;
public:
	ItemBase();
	~ItemBase();

public:
	int16 itemId_; //物品ID
	std::string itemName_; //物品名字
	int16 functionType_; // 处理函数类型
	int16 bufNum_;
	int16 bufList_[sMaxBufNum];
private:
	DISALLOW_COPY_AND_ASSIGN(ItemBase);
};

#endif /* ITEMBASE_H_ */
