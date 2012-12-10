
#ifndef GAME_LUAWORKERTHREAD_H_
#define GAME_LUAWORKERTHREAD_H_

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
class LuaSendPB
{
public:
	static int openSendPB(lua_State* L);
};

#endif
