
#ifndef GAME_LUACLIENT_H_
#define GAME_LUACLIENT_H_

#include "lua.hpp"
#include "DBLuaClient.h"
class LuaClient
{
public:
	static void pushTcpConnection(lua_State* L, mysdk::net::TcpConnection* pCon);
	static int openlib(lua_State* L);
};

#endif
