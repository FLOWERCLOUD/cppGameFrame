
#include "LuaWK.h"
#include "LuaPB.h"

#include <google/protobuf/message.h>
#include <game/dbsrv/WorkerThread.h>

static int wk_send(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TNUMBER);
    int conId = static_cast<int>(lua_tointeger(L, 1));

	luaL_checkudata(L, 2, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 2));
    luaL_argcheck(L, luamsg != NULL, 2, "wk_send msg userdata expected");

    lua_getglobal(L, "sThread");
    if (!lua_isuserdata(L, -1))
    {
    	luaL_argerror(L, 0, "sThread  expected");
    	return 0;
    }

    WorkerThread* wt = static_cast<WorkerThread *>(lua_touserdata(L, -1));
    google::protobuf::Message *message = luamsg->msg;

    wt->sendReplyEx(conId, *message);
	return 0;
}

enum
{
	kAtTime = 1,
	kEventTime = 2,
};

static int wk_startTime(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TNUMBER);
	double delay = lua_tonumber(L, 1);

	luaL_checktype(L, 2, LUA_TSTRING);
	const char* funname = lua_tostring(L, 2);

	luaL_checktype(L, 3, LUA_TNUMBER);
	int timetype = static_cast<int>(lua_tointeger(L, 3));

    lua_getglobal(L, "sThread");
    if (!lua_isuserdata(L, -1))
    {
    	luaL_argerror(L, 0, "sThread  expected");
    	return 0;
    }

    WorkerThread* wk = static_cast<WorkerThread *>(lua_touserdata(L, -1));

	if (timetype == kAtTime)
	{
		wk->startTime(delay, funname);
	}
	else if (timetype == kEventTime)
	{
		wk->startTime(delay, funname, false);
	}
	else
	{
		luaL_argerror(L , 3, "wk_startTime timetype error, 'WK_TIME_ATTIME' or 'WK_TIME_EVENTTIME' expected");
	}
	return 0;
}

static const struct luaL_reg wklib_f[] =
{
		{"send", wk_send},
		{"startTime", wk_startTime},
		{NULL, NULL}
};

int LuaWK::openlib(lua_State* L)
{
	luaL_register(L, "wk", wklib_f);

	lua_pushnumber(L, kAtTime);
	lua_setglobal(L, "WK_TIME_ATTIME");

	lua_pushnumber(L, kEventTime);
	lua_setglobal(L, "WK_TIME_EVENTTIME");

	return 1;
}
