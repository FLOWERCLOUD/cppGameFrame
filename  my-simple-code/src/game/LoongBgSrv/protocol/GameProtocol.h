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
	static const uint32 LOGIN					=	0x00120001; //登陆战场服务器
	static const uint32 ENTER_BATTLE	=	0x00120002; //进入莫个战场
	static const uint32 MOVE					= 0x00120003; //玩家移动
	static const uint32 CHAT					= 0x00120004; //聊天
}

namespace client
{
	static const uint32 LOGIN					=	0x00140001;
	static const uint32 ENTER_BATTLE	=	0x00140002; //进入莫个战场
	static const uint32 MOVE					= 0x00140003; //玩家移动
	static const uint32 CHAT					= 0x00140004; //聊天
}

#endif /* GAMEPROTOCOL_H_ */
