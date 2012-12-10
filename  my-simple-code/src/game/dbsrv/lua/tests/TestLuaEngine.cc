
#include "../LuaEngine.h"

int main()
{
	LuaEngine luaEngine;
	luaEngine.openlibs();
	if (luaEngine.dofile("test.lua") == 0)
	{
		int rt = luaEngine.callfunc("test", "");
		printf("call func ret:%d\n", rt);
	}
	else
	{
		printf("read test.lua error!!!\n");
	}
	return 0;
}
