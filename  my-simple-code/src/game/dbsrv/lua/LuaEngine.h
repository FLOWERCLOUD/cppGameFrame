
#ifndef GAME_LUAENGINE_H_
#define GAME_LUAENGINE_H_

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <stdarg.h>

class LuaEngine
{
public:
	LuaEngine();
	~LuaEngine();

	void openlibs();
public:
	int dofile(const char *filename);
	int dostring(const char *s);

	int callfunc(const char *funcname, const char *format, ...);

	lua_State* getLuaState()
	{
		return luaState_;
	}
private:
	lua_State* luaState_;
};

#endif /* LUAENGINE_H_ */
