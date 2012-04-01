/*
 * Item.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_ITEM_H_
#define GAME_ITEM_H_

#include <mysdk/base/Common.h>

using namespace mysdk;
class Item
{
public:
	Item();
	Item(int16 itemId, int16 num);
	~Item();

	int16 getItemId();
	int16 getNum();
	void setItemId(int16 itemId);
	void setItemNum(int16 num);
private:
	int16 itemId_; // 物品ID
	int16 num_; // 物品数量
private:
	DISALLOW_COPY_AND_ASSIGN(Item);
};

#endif /* ITEM_H_ */
