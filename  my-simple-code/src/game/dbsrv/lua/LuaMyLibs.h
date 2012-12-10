
#ifndef GAME_LUAMYLIBS_H_
#define GAME_LUAMYLIBS_H_

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class LuaMyLibs
{
public:
	static void openmylibs(lua_State *L);
};

#endif
