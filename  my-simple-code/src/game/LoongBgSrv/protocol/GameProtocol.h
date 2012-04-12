/*
 * GameProtocol.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_GAMEPROTOCOL_H_
#define GAME_GAMEPROTOCOL_H_

#include <mysdk/base/Common.h>

namespace game
{
	static const uint32 OP_LOGIN						=	0x00120001; //登陆战场服务器
	static const uint32 OP_ENTER_BATTLE		=	0x00120002; //进入莫个战场
	static const uint32 OP_MOVE						= 0x00120003; //玩家移动
	static const uint32 OP_CHAT						= 0x00120004; //聊天
	static const uint32 OP_REQ_BATTLE_INFO	= 0x00120005; //请求战场信息
	static const uint32 OP_STAND						= 0x00120006; //客户端上报坐标点
	// 0x00120007
	// 0x00120008
	static const uint32 OP_REQ_PLAYER_LIST	= 0x00120009; //请求战场玩家列表
	static const uint32 OP_SELCET_PET				= 0x0012000A; //选择宠物
	static const uint32 OP_USE_SKILL				= 0x0012000B; //使用技能
	// 0x0012000C
	// 0x0012000D
	// 0x0012000E
	static const uint32 OP_EXIT_BATTLE			= 0x00120010; //退出战场
	static const uint32 OP_PICKUP_ITEM			= 0x00120011; //拾取物品
	static const uint32 OP_USE_ITEM					= 0x00120012; //使用物品
	// 0x00120013
	// 0x00120014
	// 0x00120015
	// 0x00120016
	// 0x00120017
	static const uint32 OP_PING				= 0x00120018; // 应用程序心跳
}

namespace client
{
	static const uint32 OP_LOGIN						=	0x00140001; //登陆战场服务器
	static const uint32 OP_ENTER_BATTLE		=	0x00140002; //进入莫个战场
	static const uint32 OP_MOVE						= 0x00140003; //玩家移动
	static const uint32 OP_CHAT						= 0x00140004; //聊天
	static const uint32 OP_REQ_BATTLE_INFO	= 0x00140005; //请求战场信息
	// 0x00140006
	static const uint32 OP_ADD_PLAYER			= 0x00140007; //有玩家进入场景
	static const uint32 OP_REMOVE_PLAYER	= 0x00140008; // 有玩家离开场景
	static const uint32 OP_REQ_PLAYER_LIST	= 0x00140009; //请求战场玩家列表
	static const uint32 OP_SELCET_PET				= 0x0014000A; //选择宠物
	static const uint32 OP_USE_SKILL				= 0x0014000B; //使用技能
	static const uint32 OP_ADD_BUF					= 0x0014000C; // 中了buf
	static const uint32 OP_REMOVE_BUF			= 0x0014000D;// 移除buf
	static const uint32 OP_ON_HURT				= 0x0014000E;// 伤害
	static const uint32 OP_EXIT_BATTLE			= 0x00140010; //退出战场
	static const uint32 OP_PICKUP_ITEM			= 0x00140011; //拾取物品
	static const uint32 OP_USE_ITEM					= 0x00140012; //使用物品
	static const uint32 OP_TELLCLIENT_STATE = 0x00140013; //战场状态
	static const uint32 OP_DROP_ITEM				 = 0x00140014; //战场掉落了一个物品
	static const uint32 OP_ALERT_CODE			 = 0x00140015; // 错误码
	static const uint32 OP_PET_DEAD		 		 = 0x00140016; // 英雄死了
	static const uint32 OP_ADD_HP		 		 	 = 0x00140017; // 英雄加血
	static const uint32 OP_PING							 = 0x00140018; // 应用程序心跳
	static const uint32 OP_SETTLEMENT			 = 0x00140019; //战场结果

}

#endif /* GAMEPROTOCOL_H_ */
