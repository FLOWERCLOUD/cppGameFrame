
#include "LuaMySql.h"

#include <game/dbsrv/mysql/MySQLConnection.h>

 #include <string.h> // for strlen

using namespace mysdk;
static int mysql_query(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    void *ptr = lua_touserdata(L, 1);
    luaL_argcheck(L, ptr != NULL, 1, "mysql_query userdata expected");

    luaL_checktype(L, 2, LUA_TSTRING);
    const char* sql = lua_tostring(L, 2);

    MySQLConnection* mysql = static_cast<MySQLConnection*>(ptr);
    MYSQL_RES *result = NULL;
    MYSQL_FIELD *fields = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;
    bool ret = mysql->query(sql, &result, &fields, &rowCount, &fieldCount);
    if (!ret || !result)
    {
    	return 0;
    }

    lua_newtable(L);
    for (uint64 i = 0; i < rowCount; i++)
    {
    	lua_newtable(L);
        MYSQL_ROW row = mysql_fetch_row(result);
        ulong* fieldLength = mysql_fetch_lengths(result);
        for (uint32 j = 0; j < fieldCount; j++)
        {
            int  len = static_cast<int>(fieldLength[j]);
        	lua_pushlstring(L, row[j], len);
        	lua_rawseti(L, -2, j + 1);
        }
        lua_rawseti(L, -2, static_cast<int>(i + 1));
    }
    mysql_free_result(result);
    return 1;
}

static int mysql_execute(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    void *ptr = lua_touserdata(L, 1);
    luaL_argcheck(L, ptr != NULL, 1, "mysql_query userdata expected");

    luaL_checktype(L, 2, LUA_TSTRING);
    const char* sql = lua_tostring(L, 2);

    MySQLConnection* mysql = static_cast<MySQLConnection*>(ptr);
    bool ret = mysql->execute(sql);
    lua_pushboolean(L, ret);
    return 1;
}

static const struct luaL_reg mysqllib_f[] =
{
		{"query", mysql_query},
		{"execute", mysql_execute},
		{NULL, NULL}
};

int LuaMySql::openlib(lua_State* L)
{
	luaL_register(L, "MySql", mysqllib_f);
	return 1;
}
