
#ifndef GAME_LUAPB_H_
#define GAME_LUAPB_H_

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class LuaPB
{
public:
	static int openpb(lua_State* L);
};

#endif
