
#ifndef GAME_LUAREDIS_H_
#define GAME_LUAREDIS_H_

#include "lua.hpp"

class LuaRedis
{
public:
	static int openlib(lua_State* L);
};

#endif
