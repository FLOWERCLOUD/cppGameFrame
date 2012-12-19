
#include "LuaRedis.h"

#include <game/dbsrv/cache/rcache.h>

#include <strings.h>

static int pushReplyObjectToLua(lua_State* L, redisReply* reply)
{
	if (reply->type == REDIS_REPLY_STATUS)
	{
		lua_pushlstring(L, reply->str, reply->len);
	}
	else if (reply->type == REDIS_REPLY_ERROR)
	{
		lua_pushlstring(L, reply->str, reply->len);
	}
	else if (reply->type == REDIS_REPLY_STRING)
	{
		lua_pushlstring(L, reply->str, reply->len);
	}
	else if (reply->type == REDIS_REPLY_INTEGER)
	{
		lua_pushnumber(L, static_cast<lua_Number>(reply->integer));
	}
	else if (reply->type == REDIS_REPLY_ARRAY)
	{
		lua_newtable(L);

        redisReply **elements = reply->element;
        for(size_t i = 0; i < reply->elements; i++)
        {
           redisReply *reply = elements[i];
           // value
           if (reply->type == REDIS_REPLY_STRING)
           {
        	   lua_pushlstring(L, reply->str, reply->len);
           }
           else if (reply->type == REDIS_REPLY_INTEGER)
           {
        	   lua_pushnumber(L, static_cast<lua_Number>(reply->integer));
           }
           else
           {
        	   char buf[100];
        	   snprintf(buf, 99, "redis_getReplyObjectResult reply type(%d) not support!!", reply->type);
        	   luaL_argerror(L, 0, buf);
           }

           lua_rawseti(L, -2, i + 1);
        }
	}
	else
	{
		return 0;
	}

	return 1;
}

static int redis_command(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    void *ptr = lua_touserdata(L, 1);
    luaL_argcheck(L, ptr != NULL, 1, "redis_command userdata expected");

    luaL_checktype(L, 2, LUA_TSTRING);
    const char* command = lua_tostring(L, 2);

	rcache::Cache* redis = static_cast<rcache::Cache*>(ptr);
	redisReply* reply = static_cast<redisReply*>(redis->RedisCommand(command));
	// 命令没有执行成功怎么处理呢？？
	if (!reply)
	{
		lua_pushnumber(L, REDIS_REPLY_ERROR);
		return 1;
	}

	lua_pushnumber(L, reply->type);
	int ret = pushReplyObjectToLua(L, reply);

	rcache::Cache::FreeReplyObject(reply);
	return 1 + ret;
}

static const struct luaL_reg redislib_f[] =
{
		{"command", redis_command},
		{NULL, NULL}
};

int LuaRedis::openlib(lua_State* L)
{
	luaL_register(L, "Redis", redislib_f);

	lua_pushnumber(L, REDIS_REPLY_STRING);
	lua_setglobal(L, "REDIS_REPLY_STRING");

	lua_pushnumber(L, REDIS_REPLY_ARRAY);
	lua_setglobal(L, "REDIS_REPLY_ARRAY");

	lua_pushnumber(L, REDIS_REPLY_INTEGER);
	lua_setglobal(L, "REDIS_REPLY_INTEGER");

	lua_pushnumber(L, REDIS_REPLY_NIL);
	lua_setglobal(L, "REDIS_REPLY_NIL");

	lua_pushnumber(L, REDIS_REPLY_STATUS);
	lua_setglobal(L, "REDIS_REPLY_STATUS");

	lua_pushnumber(L, REDIS_REPLY_ERROR);
	lua_setglobal(L, "REDIS_REPLY_ERROR");

	return 1;
}
