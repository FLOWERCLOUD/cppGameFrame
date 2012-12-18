
#include "LuaRedis.h"

#include <game/dbsrv/cache/rcache.h>

#include <strings.h>

static int redis_command(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    void *ptr = lua_touserdata(L, 1);
    luaL_argcheck(L, ptr != NULL, 1, "redis_command userdata expected");

    luaL_checktype(L, 2, LUA_TSTRING);
    const char* command = lua_tostring(L, 2);

	rcache::Cache* redis = static_cast<rcache::Cache*>(ptr);
	void* reply = redis->RedisCommand(command);
	// 命令没有执行成功怎么处理呢？？
	if (!reply)
	{
		lua_pushnil(L);
		return 1;
	}

	void** tmp = static_cast<void**>(lua_newuserdata(L, sizeof(void*)));
	*tmp = reply;

	luaL_getmetatable(L, "redisReply");
	lua_setmetatable(L, -2);
	return 1;
}

static int redis_freeReplyObject(lua_State* L)
{
	luaL_checkudata(L, 1, "redisReply");

	void* ptr = *(static_cast<void**>(lua_touserdata(L, 1)));
	luaL_argcheck(L, ptr != NULL, 1, "redis_freeReplyObject userdata expected");

	redisReply*reply = static_cast<redisReply *>(ptr);
	rcache::Cache::FreeReplyObject(reply);

	printf("redis_freeReplyObject\n");
	return 0;
}

static int redis_getReplyObjectType(lua_State* L)
{
	luaL_checkudata(L, 1, "redisReply");

	void* ptr = *(static_cast<void**>(lua_touserdata(L, 1)));
	luaL_argcheck(L, ptr != NULL, 1, "redis_getReplyObjectType userdata expected");

	redisReply*reply = static_cast<redisReply *>(ptr);
    luaL_checktype(L, 2, LUA_TSTRING);
    const char* cmd = lua_tostring(L, 2);
    if (strcasecmp(cmd, "type") == 0)
    {
        lua_pushnumber(L, reply->type);
        return 1;
    }

    luaL_argerror(L, 2, "redis_getReplyObjectType 'type'  expected");
    return 0;
}

static int redis_getReplyObjectResult(lua_State* L)
{
	luaL_checkudata(L, 1, "redisReply");

	void* ptr = *(static_cast<void**>(lua_touserdata(L, 1)));
	luaL_argcheck(L, ptr != NULL, 1, "redis_getReplyObjectResult userdata expected");

	redisReply*reply = static_cast<redisReply *>(ptr);

	if (reply->type == REDIS_REPLY_STRING)
	{
		lua_pushlstring(L, reply->str, reply->len);
		return 1;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		lua_newtable(L);

        redisReply **elements = reply->element;
        printf("redis reply result: %d\n", reply->elements);
        for(size_t i = 0; i < reply->elements; i++)
        {
           redisReply *reply = elements[i];
           // key
           lua_pushnumber(L, i + 1);
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
        	   //lua_pushnil(L);
           }

           lua_settable(L, -3);
        }

		return 1;
	}

	if (reply->type == REDIS_REPLY_INTEGER)
	{
		lua_pushnumber(L, static_cast<lua_Number>(reply->integer));
		return 1;
	}

	if (reply->type == REDIS_REPLY_NIL)
	{
		lua_pushnil(L);
		return 1;
	}

	if (reply->type == REDIS_REPLY_STATUS)
	{
		lua_pushlstring(L, reply->str, reply->len);
		return 1;
	}

	if (reply->type == REDIS_REPLY_ERROR)
	{
		lua_pushlstring(L, reply->str, reply->len);
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static const struct luaL_reg redisreplylib_m[] =
{
		{"__gc", redis_freeReplyObject},
		{"__index", redis_getReplyObjectType},
		{NULL, NULL}
};

static const struct luaL_reg redisreplylib_f[] =
{
		{"result", redis_getReplyObjectResult},
		{NULL, NULL}
};

static const struct luaL_reg redislib_f[] =
{
		{"command", redis_command},
		{NULL, NULL}
};

int LuaRedis::openlib(lua_State* L)
{
	luaL_newmetatable(L, "redisReply");
	luaL_register(L, NULL, redisreplylib_m);
	luaL_register(L, "RedisReply", redisreplylib_f);

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
