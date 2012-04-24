/*
 * GameProtocol.h
 *
 *  Created on: 2012-3-22
 *    
 */

#ifndef GAME_GMPROTOCOL_H_
#define GAME_GMPROTOCOL_H_

#include <mysdk/base/Common.h>

namespace gmgame
{
	static const uint32 OP_GM_CMD						=	0x00130001; //gm 命令
}

namespace gmclient
{
	static const uint32 OP_GM_CMD						=	0x00150001; //gm 命令
}

#endif /* GAMEPROTOCOL_H_ */
