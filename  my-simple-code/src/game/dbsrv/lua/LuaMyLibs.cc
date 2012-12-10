
#include "LuaMyLibs.h"

#include "LuaPB.h"
#include "LuaSendPB.h"

static const luaL_Reg lualibs[] =
{
  {"pb", LuaPB::openpb},
  {"wk", LuaSendPB::openSendPB},
  {NULL, NULL}
};


void LuaMyLibs::openmylibs(lua_State *L)
{
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++)
  {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}
