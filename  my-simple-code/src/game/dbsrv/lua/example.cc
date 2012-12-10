
#include "LuaBind.h"

Lfunc(Entity_new)
 Linst(Entity)
 Lret1
 
// Yes, there are THREE (3) underscores
Lfunc(Entity___gc)
 luaL_checktype(ls, 1, LUA_TUSERDATA);
 Entity * self = (Entity*)lua_touserdata(ls, 1);
 self->~Entity();
 Lret0
 
Lmeth(Entity, get_location)
 Lretv(self->location())
 
Lmeth(Entity, move)
 Lparm(LVec2i, dist)
 const Vec2i pos = self->move(dist);
 Lretv(pos)
 
// This function shows how you can manually parse a Lua table
Lmeth(Entity, load_sprite)
 Lparm(Lstring, ident)
 
 if(self->sprite(ident))
 {
  cerr << "Unable to load sprite '" << ident 
    << "', a sprite is already using that identifier." << endl;
 }
 else
 {
  // Manually extract the table parameter.
  luaL_checktype(ls, _stackpos, LUA_TTABLE);
  vector<string> textures;
  textures.reserve(lua_objlen(ls, _stackpos));

  lua_pushnil(ls);
  while(lua_next(ls, _stackpos))
  {
   // key is at -2, val is at -1
   textures.push_back(lua_tostring(ls, -1));
   lua_pop(ls, 1);
  }
  
  auto_ptr<Sprite> sprite(new Sprite);
  for(vector<string>::iterator i = textures.begin(); 
   i != textures.end(); ++i)
  {
   sprite->addFrame(*i);
  }
  
  if(self->insertSprite(ident, sprite.get()))
   sprite.release(); 
 }
 Lret0
 
Lmeth(Entity, set_sprite)
 Lparm(Lstring, ident)
 self->activateSprite(ident);
 Lret0
 
// Define the interface to 'Entity'
Linterface_start(Entity)
 Linterface(Entity, __gc)
 Linterface(Entity, move)
 Linterface(Entity, get_location)
 Linterface(Entity, load_sprite)
 Linterface(Entity, set_sprite)
Linterface_end

void LuaExport (lua_State * ls)
{
 // Export the 'Entity' interface
 Linterface_class(Entity)
Linterface_group(Entity)
}

