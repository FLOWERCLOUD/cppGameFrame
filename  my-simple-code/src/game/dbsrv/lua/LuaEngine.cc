
#include "LuaEngine.h"

#include <stdio.h>
#include <stdarg.h>


LuaEngine::LuaEngine()
{
	luaState_ = lua_open();
	if (luaState_ == NULL)
	{
		 fprintf(stderr,"not enough memory for state\n");
		 return;
	}
}

LuaEngine::~LuaEngine()
{
	 lua_close(luaState_);
}

void LuaEngine::openlibs()
{
	lua_gc(luaState_, LUA_GCSTOP, 0);  /* stop collector during initialization */
	luaL_openlibs(luaState_);  /* open libraries */
	lua_gc(luaState_, LUA_GCRESTART, 0);
}

#include <stdlib.h>

int LuaEngine::dofile(const char *filename)
{
	char* luapath = getenv("MYLUA_PATH");
	if (!luapath)
	{
		 if(luaL_dofile(luaState_, filename) != 0)
		 {
			 //
			 printf("[LuaEngine] LuaEngine::dofile(%s) error!!\n", filename);
			 exit(1);
		 }
	}
	else
	{
		printf("[LuaEngine] LuaEngine::dofile lua path is: %s\n",luapath);
		char name[1024];
		snprintf(name, 1023, "%s/%s", luapath, filename);
		if(luaL_dofile(luaState_, name) != 0)
		{
			printf("[LuaEngine] LuaEngine::dofile(%s) error, result:%s!!\n", name, lua_tostring(luaState_, -1));
			exit(1);
		}
	}
	return 0;
}

int LuaEngine::dostring(const char *s)
{
	return luaL_dostring(luaState_, s);
}

int LuaEngine::callfunc(const char *funcname, const char *format, ...)
{
	lua_getglobal(luaState_, funcname);
	luaL_checktype(luaState_, 1, LUA_TFUNCTION);

	int d;
	int arg_num = 0;
    va_list ap;
    va_start(ap,format);
    while (*format)
    {
        switch (*format++)
        {
        case 's':              /* string */
            lua_pushstring(luaState_, va_arg(ap, char *));
            arg_num++;
            break;
        case 'd':              /* int */
        	d = va_arg(ap, int);
        	lua_pushinteger(luaState_, d);
        	arg_num++;
            break;
        case 'f':
        	lua_pushnumber(luaState_, va_arg(ap, double));
        	arg_num++;
        	break;
        case 'u':
        	lua_pushlightuserdata(luaState_, va_arg(ap, void*));
        	arg_num++;
        	break;
        }
    }
    va_end(ap);

	if (lua_pcall(luaState_, arg_num, 1, 0) != 0)
	{
		fprintf(stderr, "lua_pcall error, funcname:%s, error msg:%s\n", funcname,
				lua_tostring(luaState_, -1));
		lua_pop(luaState_, 1);// 从栈中弹出出错消息
		return -1;
	}

	int ir = lua_gettop(luaState_);

	if (ir > 0)
	{
		int tr = static_cast<int>(lua_tonumber(luaState_, -1));
		lua_pop(luaState_, ir);
		return tr;
	}

    return 0;
}
