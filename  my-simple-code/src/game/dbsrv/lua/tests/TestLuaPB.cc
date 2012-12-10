
#include <google/protobuf/stubs/common.h>

#include "../LuaEngine.h"
#include "../LuaPB.h"

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	LuaEngine luaEngine;
	luaEngine.openlibs();
	//LuaPB::openpb(luaEngine.getLuaState());
	if (luaEngine.dofile("test.lua") == 0)
	{
		int rt = luaEngine.callfunc("test", "%d", 1);
		printf("call func ret:%d\n", rt);
	}
	else
	{
		printf("read test.lua error!!!\n");
	}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
