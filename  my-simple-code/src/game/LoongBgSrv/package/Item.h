/*
 * Item.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#include <mysdk/base/Common.h>

class Item
{
public:
	Item(int16 itemId, int16 num, int16 type);
	~Item();

	int16 getItemId();
	int16 getNum();
	int16 getType();
private:
	int16 itemId_; // 物品ID
	int16 num_; // 物品数量
	int16 type_; //物品类型
private:
	DISALLOW_COPY_AND_ASSIGN(Item);
};

#endif /* ITEM_H_ */
