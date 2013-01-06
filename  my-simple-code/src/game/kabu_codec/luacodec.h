#ifndef LUACODEC_H_
#define LUACODEC_H_

#include "lua.hpp"

typedef struct tagluacodec
{
	int on_data_cb;
	int on_error_cb;
}lua_codec;

#define LUA_CODEC_META "luacodec_meta"
#define LUA_CODEC "luacodec"

extern "C" int  luaopen_luacodec(lua_State* L);

#endif
