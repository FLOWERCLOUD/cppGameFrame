
#include "LuaSendPB.h"

#include <google/protobuf/message.h>

#include <game/dbsrv/WorkerThread.h>

static int wk_send(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    void *ptr = lua_touserdata(L, 1);
    luaL_argcheck(L, ptr != NULL, 1, "wk_send workthread userdata expected");

    luaL_checktype(L, 2, LUA_TNUMBER);
    int conId = lua_tointeger(L, 2);

	luaL_checkudata(L, 3, "pb");
    void *msg = *(static_cast<void**>(lua_touserdata(L, 3)));
    luaL_argcheck(L, msg != NULL, 2, "wk_send msg userdata expected");

    WorkerThread* thread = static_cast<WorkerThread *>(ptr);
    google::protobuf::Message *message = static_cast<google::protobuf::Message *>(msg);

    thread->sendReplyEx(conId, *message);
	return 0;
}

static const struct luaL_reg wklib_f[] =
{
		{"send", wk_send},
		{NULL, NULL}
};

int LuaSendPB::openSendPB(lua_State* L)
{
	luaL_register(L, "wk", wklib_f);
	return 1;
}
