
#include "LuaPB.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/importer.h>

class MyMultiFileErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
        virtual void AddError(
                const std::string & filename,
                int line,
                int column,
                const std::string & message)
        {
        	fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
       }
};

static MyMultiFileErrorCollector errorCollector;
static google::protobuf::compiler::DiskSourceTree sourceTree;
static google::protobuf::compiler::Importer importer(&sourceTree, &errorCollector);
static google::protobuf::DynamicMessageFactory factory;

static int pb_get(lua_State* L)
{
	luaL_checkudata(L, 1, "pb");

    void *ptr = *(static_cast<void**>(lua_touserdata(L, 1)));

    luaL_argcheck(L, ptr != NULL, 1, "luapb::get userdata expected");

	luaL_checktype(L, 2, LUA_TSTRING);
    const char *field_name = lua_tostring(L, 2);

    google::protobuf::Message *message = static_cast<google::protobuf::Message *>(ptr);

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor *field = descriptor->FindFieldByName(field_name);

    luaL_argcheck(L, field != NULL, 2, "luapb::get field_name error");
    luaL_argcheck(L, !field->is_repeated(), 2, "luapb::get field_name is repeated");

    if(field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    {
        google::protobuf::Message *field_message =reflection->MutableMessage(message,
        		field,
        		google::protobuf::MessageFactory::generated_factory());

    	google::protobuf::Message ** tmp = static_cast<google::protobuf::Message**>(lua_newuserdata(L, sizeof(google::protobuf::Message *)));
    	*tmp = field_message;
    	luaL_getmetatable(L, "pb");
    	lua_setmetatable(L, -2);

        //lua_pushlightuserdata(L, field_message);
    }
    else if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
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
    return 1;
}

static int pb_set(lua_State* L)
{
	luaL_checkudata(L, 1, "pb");

    void *ptr = *(static_cast<void**>(lua_touserdata(L, 1)));

    luaL_argcheck(L, ptr != NULL, 1, "LuaPB::set userdata expected");

	luaL_checktype(L, 2, LUA_TSTRING);
    const char *field_name = lua_tostring(L, 2);

    google::protobuf::Message *message = static_cast<google::protobuf::Message *>(ptr);

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
	 const  google::protobuf::FileDescriptor* filedescriptor = importer.Import(filename);
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
	  const google::protobuf::Descriptor* descriptor = importer.pool()->FindMessageTypeByName(name);
	  if (descriptor)
	  {
	     const google::protobuf::Message* prototype = factory.GetPrototype(descriptor);
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

	google::protobuf::Message ** tmp = static_cast<google::protobuf::Message**>(lua_newuserdata(L, sizeof(google::protobuf::Message *)));
	*tmp = message;
	luaL_getmetatable(L, "pb");
	lua_setmetatable(L, -2);
	return 1;
}

static int pb_delete(lua_State* L)
{
	luaL_checkudata(L, 1, "pb");

    void *ptr = *(static_cast<void**>(lua_touserdata(L, 1)));
    google::protobuf::Message *message = static_cast<google::protobuf::Message *>(ptr);
    delete message;

    printf("pb delete\n");
    return 0;
}

static const struct luaL_reg pblib_f[] =
{
		{"new", pb_new},
		{"import", pb_import},
		{"get", pb_get},
		{"set", pb_set},
		{"delete", pb_delete},
		{NULL, NULL}
};

static const struct luaL_Reg pblib_m[] =
{
		{"__index", pb_get},
		{"__newindex", pb_set},
		{"__gc", pb_delete},
		{NULL, NULL}
};

int LuaPB::openpb(lua_State* L)
{
	luaL_newmetatable(L, "pb");
	luaL_register(L, NULL, pblib_m);
	luaL_register(L, "pb", pblib_f);

    sourceTree.MapPath("", "");

	return 1;
}
