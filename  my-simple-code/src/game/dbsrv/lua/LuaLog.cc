
#include "LuaLog.h"

#include <mysdk/base/Logging.h>

#include <string>

using namespace mysdk;
static int log(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int type = static_cast<int>(lua_tonumber(L, 1));

    luaL_checktype(L, 2, LUA_TSTRING);
    const char* log = lua_tostring(L, 2);

    if (type == mysdk::Logger::DEBUG)
    {
    	LOG_DEBUG << log;
    }
    else if (type == mysdk::Logger::INFO)
    {
        LOG_INFO << log;
    }

    LOG_TRACE << log;
    return 0;
}

static const struct luaL_reg loglib_f[] =
{
		{"log", log},
		{NULL, NULL}
};

int LuaLog::openlib(lua_State* L)
{
	luaL_register(L, "Log", loglib_f);

	lua_pushnumber(L, mysdk::Logger::DEBUG);
	lua_setglobal(L, "LOG_DEBUG");

	lua_pushnumber(L, mysdk::Logger::INFO);
	lua_setglobal(L, "LOG_INFO");

	return 1;
}
