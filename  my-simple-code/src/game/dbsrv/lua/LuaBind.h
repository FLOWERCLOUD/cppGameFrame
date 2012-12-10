
#ifndef GAME_LUABIND_H_
#define GAME_LUABIND_H_

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <string>

template<typename T>
T parm_get (lua_State *, int&);

template<typename T>
T opts_get (lua_State *, int&, T const&, bool&);

template<typename T>
int ret_push (lua_State *, T const&);

//
// bool
//
typedef bool Lparm_Lbool;
template<> bool parm_get (lua_State * ls, int& stackpos)
{
 luaL_checktype(ls, stackpos, LUA_TBOOLEAN);
 return lua_toboolean(ls, stackpos++);
}
template<> bool opts_get (lua_State * ls, int& stackpos, bool const& defaultval, bool& look)
{
 if(look && lua_isboolean(ls, stackpos))
  return lua_toboolean(ls, stackpos++);

 look = false;
 return defaultval;
}
template<> int ret_push (lua_State * ls, bool const& b)
{
 lua_pushboolean(ls, b);
 return 1;
}

//
// int
//
typedef int Lparm_Lint;
template<> int parm_get (lua_State * ls, int& stackpos)
{
 luaL_checktype(ls, stackpos, LUA_TNUMBER);
 return static_cast<int>(lua_tointeger(ls, stackpos++));
}
template<> int opts_get (lua_State * ls, int& stackpos, int const& defaultval, bool& look)
{
 if(look && lua_isnumber(ls, stackpos))
  return static_cast<int>(lua_tointeger(ls, stackpos++));

 look = false;
 return defaultval;
}
template<> int ret_push (lua_State * ls, int const& n)
{
 lua_pushinteger(ls, n);
 return 1;
}

//
// float
//
typedef float Lparm_Lfloat;
template<> float parm_get (lua_State * ls, int& stackpos)
{
 luaL_checktype(ls, stackpos, LUA_TNUMBER);
 return static_cast<float>(lua_tonumber(ls, stackpos++));
}
template<> float opts_get (lua_State * ls, int& stackpos, float const& defaultval, bool& look)
{
 if(look && lua_isnumber(ls, stackpos))
  return static_cast<float>(lua_tonumber(ls, stackpos++));

 look = false;
 return defaultval;
}
template<> int ret_push (lua_State * ls, float const& f)
{
 lua_pushnumber(ls, f);
 return 1;
}

//
// string
//
typedef std::string Lparm_Lstring;
template<> std::string parm_get (lua_State * ls, int& stackpos)
{
 luaL_checktype(ls, stackpos, LUA_TSTRING);
 return std::string(lua_tostring(ls, stackpos++));
}
template<> std::string opts_get (lua_State * ls, int& stackpos, std::string const& defaultval, bool& look)
{
 if(look && lua_isstring(ls, stackpos))
  return std::string(lua_tostring(ls, stackpos++));

 look = false;
 return defaultval;
}
template<> int ret_push (lua_State * ls, std::string const& s)
{
 lua_pushstring(ls, s.c_str());
 return 1;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define Lfunc(NAME) \
 int NAME (lua_State * ls) { \
  int _stackpos = 1; (void)_stackpos; \
  bool _opts = true; (void)_opts; \
  try {

#define Lself(TYPE) \
 luaL_checktype(ls, 1, LUA_TTABLE); \
 lua_getfield(ls, 1, "_core"); \
 lua_insert(ls, 2); \
 TYPE * self = (TYPE*)lua_touserdata(ls, 2); (void)self; \
 _stackpos+=2; \

#define Lmeth(TYPE, NAME) \
 int TYPE##_##NAME (lua_State* ls) { \
  int _stackpos = 1; (void)_stackpos; \
  bool _opts = true; (void)_opts; \
  Lself(TYPE) \
  try {

#define Linst(TYPE) \
 lua_newtable(ls); \
 luaL_getmetatable(ls, #TYPE); \
 lua_setmetatable(ls, -2); \
 TYPE * inst = new (lua_newuserdata(ls, sizeof(TYPE))) TYPE; \
 (void*)inst; \
 luaL_getmetatable(ls, #TYPE); \
 lua_setmetatable(ls, -2); \
 lua_setfield(ls, -2, "_core");

#define Lparm(TYPE, NAME) \
 Lparm##_##TYPE NAME = parm_get<Lparm##_##TYPE>(ls, _stackpos);

#define Lopts(TYPE, NAME, DEFVAL) \
 Lparm##_##TYPE NAME = opts_get<TYPE>(ls, _stackpos, DEFVAL, _opts);

#define Lcatch \
 } catch (std::exception const& err) { \
  std::cerr << err.what() << std::endl; \
 } catch (...) { \
  std::cerr << "unknown exception." << endl; \
 }

#define Lret0 \
 Lcatch \
 lua_settop(ls, 0); \
 return 0; \
 }

#define Lret1 \
 Lcatch \
 lua_insert(ls, 1); \
 lua_settop(ls, 1); \
 return 1; \
 }

#define Lret2 \
 Lcatch \
 lua_insert(ls, 1); \
 lua_insert(ls, 2); \
 lua_settop(ls, 2); \
 return 2; \
 }

#define Lretn(N) \
 Lcatch \
 for(int i=1; i <= N; ++i)\
  lua_insert(ls, i); \
 lua_settop(ls, N); \
 return N; \
 }

#define Lretv(RET) \
 Lcatch \
 lua_settop(ls, 0); \
 return ret_push(RET); \
 }
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define Linterface_start(TYPE) \
 static const luaL_reg TYPE##_interface[] = {

#define Linterface(TYPE, FUNC) \
 {#FUNC, TYPE##_##FUNC},

#define Linterface_end \
 {0, 0}};

#define Linterface_class(TYPE) \
 lua_createtable(ls, 0, 1); \
 lua_pushcfunction(ls, TYPE##_new); \
 lua_setfield(ls, -2, "new"); \
 lua_setfield(ls, LUA_GLOBALSINDEX, #TYPE); \
 \
    luaL_newmetatable(ls, #TYPE); \
    lua_pushstring(ls, "__index"); \
    lua_pushvalue(ls, -2); \
    lua_settable(ls, -3); \
 \
    luaL_openlib(ls, 0, TYPE##_interface, 0);

#define Linterface_group(NAME) \
 luaL_openlib(ls, #NAME, NAME##_interface, 0);


#endif /* LUABIND_H_ */
