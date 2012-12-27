
#include "LuaClient.h"


#include <game/dbsrv/lua/LuaPB.h>

#include <google/protobuf/message.h>

static int send(lua_State* L)
{
	//luaL_checktype(L, 1, LUA_TTABLE);
	//lua_getfield(L, 1, "TcpConnection_core");

    mysdk::net::TcpConnection* pCon = static_cast<mysdk::net::TcpConnection*>(lua_touserdata(L, 1));

	luaL_checkudata(L, 2, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 2));
    luaL_argcheck(L, luamsg != NULL, 2, "wk_send msg userdata expected");

    lua_getglobal(L, "sClient");
    if (!lua_isuserdata(L, -1))
    {
    	luaL_argerror(L, 0, "sClient  expected");
    	return 0;
    }

    google::protobuf::Message *msg = luamsg->msg;
    DBLuaClient* client = static_cast<DBLuaClient *>(lua_touserdata(L, -1));
    client->send(pCon, msg);
	return 0;
}

void LuaClient::pushTcpConnection(lua_State* L, mysdk::net::TcpConnection* pCon)
{
	lua_newtable(L);
	lua_pushlightuserdata(L, pCon);
	lua_setfield(L, -2, "TcpConnection_core");
}

static const struct luaL_reg lib_f[] =
{
		{"send", send},
		{NULL, NULL}
};

int LuaClient::openlib(lua_State* L)
{
	luaL_register(L, "client", lib_f);
	return 1;
}
