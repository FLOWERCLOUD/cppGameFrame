
#include "LuaPB.h"

#include <game/dbsrv/ProtoImporter.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/importer.h>


static int pb_repeated_get(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_getfield(L, 1, LuaPB::sRepeatedMessageCore);

	LuaRepeatedMessage* repeated  = static_cast<LuaRepeatedMessage*>(lua_touserdata(L, -1));
    luaL_argcheck(L, repeated != NULL, 1, "pb_repeated_get LuaRepeatedMessage* is null");

    google::protobuf::Message* message = repeated->msg;
    google::protobuf::FieldDescriptor *field = repeated->field;
    const google::protobuf::Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_get field not exist");

    luaL_argcheck(L, lua_isnumber(L, 2), 2, "pb_repeated_get index expected int");
    // -1 为了和lua的下标从一开始保持一致
	int index = static_cast<int>(lua_tonumber(L, 2)) - 1;
	luaL_argcheck(L, index >= 0, 2, "pb_repeated_get index expected >= 1");
	if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
	{
		lua_pushnumber(L, reflection->GetRepeatedInt32(*message, field, index));
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
	{
		lua_pushstring(L, reflection->GetRepeatedString(*message, field, index).data());
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
	{
		lua_pushnumber(L, reflection->GetRepeatedUInt32(*message, field, index));
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
	{
		lua_pushnumber(L, reflection->GetRepeatedFloat(*message, field, index));
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
	{
		lua_pushnumber(L, reflection->GetRepeatedDouble(*message, field, index));
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
	{
		lua_pushboolean(L, reflection->GetRepeatedBool(*message, field, index));
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
	{
		const google::protobuf::Message& repeatedMessage = reflection->GetRepeatedMessage(*message, field, index);
		LuaMessage* luamsg = static_cast<LuaMessage*>(lua_newuserdata(L, sizeof(LuaMessage)));
		luamsg->msg = const_cast<google::protobuf::Message*>(&repeatedMessage);
		luamsg->isDelete = false;
		luaL_getmetatable(L, LuaPB::sMessageMeta);
		lua_setmetatable(L, -2);
	}
	else
	{
		luaL_argerror(L, 0, "pb_repeated_get field type for get not support!!!");
		return 0;
	}
	return 1;
}

static int pb_repeated_set(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_getfield(L, 1, LuaPB::sRepeatedMessageCore);

	LuaRepeatedMessage* repeated  = static_cast<LuaRepeatedMessage*>(lua_touserdata(L, -1));
    luaL_argcheck(L, repeated != NULL, 1, "pb_repeated_set LuaRepeatedMessage* is null");

    google::protobuf::Message *message = repeated->msg;
    const google::protobuf::Reflection* reflection = message->GetReflection();
    google::protobuf::FieldDescriptor *field = repeated->field;

    luaL_argcheck(L, field != NULL, 1, "pb_repeated_set field not exist");
    luaL_argcheck(L, lua_isnumber(L, 2), 2, "pb_repeated_set index expected int");

	int index = static_cast<int>(lua_tonumber(L, 2)) - 1;
	luaL_argcheck(L, index >= 0, 2, "pb_repeated_set index expected >= 1");

	if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
	{
		int val = static_cast<int>(lua_tonumber(L, 3));
		reflection->SetRepeatedInt32(message, field, index, val);
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
	{
		unsigned int val = static_cast<unsigned int>(lua_tonumber(L, 3));
		reflection->SetRepeatedUInt32(message, field, index, val);
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
	{
		float val = static_cast<float>(lua_tonumber(L, 3));
		reflection->SetRepeatedFloat(message, field, index, val);
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
	{
		double val = static_cast<double>(lua_tonumber(L, 3));
		reflection->SetRepeatedDouble(message, field, index, val);
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
	{
		int val = static_cast<int>(lua_toboolean(L, 3));
		reflection->SetRepeatedBool(message, field, index, val);
	}
	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
	{
		luaL_checktype(L, 3, LUA_TSTRING);
		const char *str = static_cast<const char *>(lua_tostring(L, 3));
		reflection->SetRepeatedString(message, field, index, str);
	}
	else
	{
		luaL_argerror(L, (2), "pb_repeated_set type for set not support!!!");
	}
	return 0;
}

static int pb_repeated_len(lua_State* L)
{
	LuaRepeatedMessage* repeated  = static_cast<LuaRepeatedMessage*>(lua_touserdata(L, lua_upvalueindex(1)));
    luaL_argcheck(L, repeated != NULL, 1, "pb_repeated_len userdata expected");

    google::protobuf::Message *message = repeated->msg;
    const google::protobuf::Reflection* reflection = message->GetReflection();
    google::protobuf::FieldDescriptor *field = repeated->field;
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_set field not exist");

    int fieldsize = reflection->FieldSize(*message, field);
    lua_pushnumber(L, fieldsize);
    return 1;
}

static int pb_repeated_add(lua_State* L)
{
	LuaRepeatedMessage* repeated  = static_cast<LuaRepeatedMessage*>(lua_touserdata(L, lua_upvalueindex(1)));
    luaL_argcheck(L, repeated != NULL, 1, "pb_repeated_get userdata expected");

    google::protobuf::Message* message = repeated->msg;
    google::protobuf::FieldDescriptor *field = repeated->field;
    const google::protobuf::Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_add field is null");

    if(field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
   {
            google::protobuf::Message* repeatedmsg = reflection->AddMessage(message, field);
            LuaMessage* luamsg = static_cast<LuaMessage*>(lua_newuserdata(L, sizeof(LuaMessage)));
            luamsg->msg = repeatedmsg;
            luamsg->isDelete = false;
        	luaL_getmetatable(L, LuaPB::sMessageMeta);
        	lua_setmetatable(L, -2);

        	return 1;
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
    	int val = static_cast<int>(lua_tonumber(L, 1));
        reflection->AddInt32(message, field, val);
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_INT64)
    {
        long val = static_cast<long>(lua_tonumber(L, 1));
        reflection->AddInt64(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
        unsigned int val = static_cast<unsigned int>(lua_tonumber(L, 1));
        reflection->AddUInt32(message, field, val);
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_UINT64)
    {
        unsigned long val = static_cast<unsigned int>(lua_tonumber(L, 1));
        reflection->AddUInt64(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
        float val = static_cast<float>(lua_tonumber(L, 1));
        reflection->AddFloat(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    {
        double val = static_cast<double>(lua_tonumber(L, 1));
        reflection->AddDouble(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
        int val = static_cast<int>(lua_toboolean(L, 1));
        reflection->AddBool(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
    	luaL_checktype(L, 1, LUA_TSTRING);
        const char *str = static_cast<const char *>(lua_tostring(L, 1));
        reflection->AddString(message, field, str);
    }
    else
    {
        	luaL_argerror(L, (1), "pb_repeated_add field name type for add  is not support!!");
     }
    return 0;
}

///////////////////////////////////////////////////////
static int pb_get(lua_State* L)
{
	luaL_checkudata(L, 1, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 1));
    luaL_argcheck(L, luamsg != NULL, 1, "pb_get userdata expected, luamsg is null");

	luaL_checktype(L, 2, LUA_TSTRING);
    const char *field_name = lua_tostring(L, 2);

    google::protobuf::Message *message = luamsg->msg;
    if (!message)
    {
    	return 0;
    }

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor *field = descriptor->FindFieldByName(field_name);
    luaL_argcheck(L, field != NULL, 2, "luapb::get field_name error");

    if (field->is_repeated())
    {
    	lua_newtable(L);

    	LuaRepeatedMessage* repeated = static_cast<LuaRepeatedMessage*>(lua_newuserdata(L, sizeof(LuaRepeatedMessage)));
    	repeated->msg = message;
    	repeated->field = const_cast<google::protobuf::FieldDescriptor *>(field);

    	// copy luaRepeatedMessage newuserdata
    	lua_pushvalue(L, -1);
    	lua_setfield(L, -3, LuaPB::sRepeatedMessageCore);

    	// copy luaRepeatedMessage newuserdata
    	lua_pushvalue(L, -1);
    	lua_pushcclosure(L, pb_repeated_len, 1);
    	lua_setfield(L, -3, "size");

    	lua_pushcclosure(L, pb_repeated_add, 1);
    	lua_setfield(L, -2, "add");

    	luaL_getmetatable(L, LuaPB::sRepeatedMessageMeta);
    	lua_setmetatable(L, -2);
    }
    else
    {
			if(field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
			{
		        google::protobuf::Message* repeatedmsg = reflection->AddMessage(message, field, sProtoImporter.factory.generated_factory());
		        LuaMessage* tmp = static_cast<LuaMessage*>(lua_newuserdata(L, sizeof(LuaMessage)));
		    	tmp->msg = repeatedmsg;
		    	tmp->isDelete = false;
		    	luaL_getmetatable(L, LuaPB::sMessageMeta);
		    	lua_setmetatable(L, -2);
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
			{
				lua_pushstring(L, reflection->GetString(*message, field).data());
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
			{
				lua_pushstring(L, reflection->GetString(*message, field).data());
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
			{
				lua_pushnumber(L, reflection->GetInt32(*message, field));
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
			{
				 lua_pushnumber(L, reflection->GetUInt32(*message, field));
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
			{
				 lua_pushnumber(L, reflection->GetFloat(*message, field));
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
			{
				 lua_pushnumber(L, reflection->GetDouble(*message, field));
			}
			else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
			{
				lua_pushboolean(L, reflection->GetBool(*message, field));
			}
    }
    return 1;
}

static int pb_set(lua_State* L)
{
	luaL_checkudata(L, 1, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 1));
    luaL_argcheck(L, luamsg != NULL, 1, "pb_get userdata expected, luamsg is null");

	luaL_checktype(L, 2, LUA_TSTRING);
    const char *field_name = lua_tostring(L, 2);

    google::protobuf::Message *message = luamsg->msg;
    if (!message)
    {
    	return 0;
    }

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor *field = descriptor->FindFieldByName(field_name);

    luaL_argcheck(L, field != NULL, 2, "LuaPB::set field_name error");
    luaL_argcheck(L, !field->is_repeated(), 2, "LuaPB::set field_name is repeated");

    if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
    	luaL_checktype(L, 3, LUA_TSTRING);
        const char *str = static_cast<const char *>(lua_tostring(L, 3));
        reflection->SetString(message, field, str);
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
    {
    	luaL_checktype(L, 3, LUA_TSTRING);
        const char *str = static_cast<const char *>(lua_tostring(L, 3));
        reflection->SetString(message, field, str);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
    {
        int val = static_cast<int>(lua_tonumber(L, 3));
        reflection->SetInt32(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
    {
        unsigned int val = static_cast<unsigned int>(lua_tonumber(L, 3));
        reflection->SetUInt32(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    {
        float val = static_cast<float>(lua_tonumber(L, 3));
        reflection->SetFloat(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    {
        double val = static_cast<double>(lua_tonumber(L, 3));
        reflection->SetDouble(message, field, val);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
    {
        int val = static_cast<int>(lua_toboolean(L, 3));
        reflection->SetBool(message, field, val);
    }
    return 0;
}

static int pb_import(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TSTRING);

	const char* filename = lua_tostring(L, 1);
	 const  google::protobuf::FileDescriptor* filedescriptor = sProtoImporter.importer.Import(filename);
	 if (!filedescriptor)
	 {
		 fprintf(stderr, "filename:%s file descriptor error\n", filename);
	 }
	 printf("name: %s, package: %s, debugstr: %s\n", filedescriptor->name().c_str(), filedescriptor->package().c_str(), filedescriptor->DebugString().c_str());

	return 0;
}

static google::protobuf::Message* create_pb_msg(const char* name)
{
	  google::protobuf::Message* message = NULL;
	  const google::protobuf::Descriptor* descriptor = sProtoImporter.importer.pool()->FindMessageTypeByName(name);
	  if (descriptor)
	  {
	     const google::protobuf::Message* prototype = sProtoImporter.factory.GetPrototype(descriptor);
	     if (prototype)
	     {
	          message = prototype->New();
	     }
	     else
	     {
	    	 fprintf(stderr, "can't find %s\n", name);
	     }
	  }
	  else
	  {
		  fprintf(stderr, "can't find descriptor %s\n", name);
	  }
	  return message;
}


static int pb_new(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TSTRING);

	const char* msgname = static_cast<const char*>(lua_tostring(L, 1));
	google::protobuf::Message *message = create_pb_msg(msgname);

    LuaMessage* tmp = static_cast<LuaMessage*>(lua_newuserdata(L, sizeof(LuaMessage)));
	tmp->msg = message;
	tmp->isDelete = true;
	luaL_getmetatable(L, LuaPB::sMessageMeta);
	lua_setmetatable(L, -2);
	return 1;
}

static int pb_delete(lua_State* L)
{
	luaL_checkudata(L, 1, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 1));
    luaL_argcheck(L, luamsg != NULL, 1, "pb_get userdata expected, luamsg is null");

    if (luamsg->isDelete)
    {
    	google::protobuf::Message*message = luamsg->msg;
    	delete message;
        printf("pb delete\n");
    }

    return 0;
}

static int pb_tostring(lua_State* L)
{
	luaL_checkudata(L, 1, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 1));
    luaL_argcheck(L, luamsg != NULL, 1, "pb_get userdata expected, luamsg is null");

    google::protobuf::Message *message = luamsg->msg;

    std::string msg(message->DebugString());
    lua_pushlstring(L, msg.c_str(), msg.length());
	return 1;
}

static int pb_serializeToString(lua_State* L)
{
	luaL_checkudata(L, 1, LuaPB::sMessageMeta);
	LuaMessage* luamsg  = static_cast<LuaMessage*>(lua_touserdata(L, 1));
    luaL_argcheck(L, luamsg != NULL, 1, "pb_get userdata expected, luamsg is null");

    google::protobuf::Message *message = luamsg->msg;

    std::string msg;
    message->SerializeToString(&msg);
    lua_pushlstring(L, msg.c_str(), msg.length());
	return 1;
}

static const struct luaL_reg pblib_f[] =
{
		{"new", pb_new},
		{"import", pb_import},
		{"get", pb_get},
		{"set", pb_set},
		{"delete", pb_delete},
		{"tostring", pb_tostring},
		{"serializeToString", pb_serializeToString},
		{NULL, NULL}
};

static const struct luaL_Reg pblib_m[] =
{
		{"__index", pb_get},
		{"__newindex", pb_set},
		{"__gc", pb_delete},
		{"__tostring", pb_tostring},
		{NULL, NULL}
};

static const struct luaL_reg pbrepeatedlib_m[] =
{
		{"__index", pb_repeated_get},
		{"__newindex", pb_repeated_set},
		{NULL, NULL}
};

int LuaPB::openlib(lua_State* L)
{
	luaL_newmetatable(L, sMessageMeta);
	luaL_register(L, NULL, pblib_m);
	luaL_register(L, "pb", pblib_f);

	luaL_newmetatable(L, sRepeatedMessageMeta);
	luaL_register(L, NULL, pbrepeatedlib_m);

	return 1;
}

int LuaPB::pushMessage(lua_State* L, google::protobuf::Message *message)
{
    LuaMessage* tmp = static_cast<LuaMessage*>(lua_newuserdata(L, sizeof(LuaMessage)));
	tmp->msg = message;
	tmp->isDelete = true;
	luaL_getmetatable(L, sMessageMeta);
	lua_setmetatable(L, -2);
	return 1;
}

const char* LuaPB::sRepeatedMessageMeta = "RepeatedMessageMeta";
const char* LuaPB::sRepeatedMessageCore = "luaRepeatedMessageCore";
const char* LuaPB::sMessageMeta = "MessageMeta";
