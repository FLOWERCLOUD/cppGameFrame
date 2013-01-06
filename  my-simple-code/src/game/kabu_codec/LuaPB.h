
#ifndef GAME_LUAPB_H_
#define GAME_LUAPB_H_

#include "lua.hpp"

#include <google/protobuf/message.h>

typedef struct tagLuaRepeatedMessage
{
	google::protobuf::Message* msg;
	google::protobuf::FieldDescriptor *field;
}LuaRepeatedMessage;

typedef struct tagLuaMessageField
{
	google::protobuf::Message* msg;
	google::protobuf::FieldDescriptor *field;
}LuaMessageField;

typedef struct tagLuaMessage
{
	google::protobuf::Message* msg;
	bool isDelete; //是否要释放掉这个msg;
}LuaMessage;



class LuaPB
{
public:
	static const char* sRepeatedMessageMeta;
	static const char* sRepeatedMessageCore;
	static const char* sMessageMeta;
	static const char* sMessageFieldMeta;
	static int pushMessage(lua_State* L, google::protobuf::Message *message);
	static int openlib(lua_State* L);
};

#endif
