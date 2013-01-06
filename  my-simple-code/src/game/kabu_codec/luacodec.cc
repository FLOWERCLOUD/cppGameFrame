
#include "luacodec.h"
#include "codec.h"
#include "LuaPB.h"
#include "luabufferevent.h"

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/util.h>

static int luacodec_new(lua_State* L)
{
	lua_codec* codec = (lua_codec*)lua_newuserdata(L, sizeof(lua_codec));
	if (!codec) return 0;

	luaL_checktype(L, 1, LUA_TFUNCTION);
	luaL_checktype(L, 2, LUA_TFUNCTION);

	lua_pushvalue(L, 1);
	codec->on_data_cb = lua_ref(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, 2);
	codec->on_error_cb = lua_ref(L, LUA_REGISTRYINDEX);

	luaL_getmetatable(L, LUA_CODEC_META);
	lua_setmetatable(L, -2);
	return 1;
}

static int luacodec_free(lua_State* L)
{
	lua_codec* codec = (lua_codec*)(luaL_checkudata(L, 1, LUA_CODEC_META));

	lua_unref(L, codec->on_data_cb);
	lua_unref(L, codec->on_error_cb);
	return 0;
}

static int luacodec_handle(lua_State* L)
{
	lua_codec* codec = (lua_codec*)(luaL_checkudata(L, 1, LUA_CODEC_META));
	lua_bufferevent* luabufferevent = (lua_bufferevent*)(luaL_checkudata(L, 2, LUA_BUFFEREVENT_META));

	struct bufferevent* bufferevent = luabufferevent->bufferevent;
	struct evbuffer* evbuf = bufferevent_get_input(bufferevent);

	while(evbuffer_get_length(evbuf) >= KabuCodec::kMinMessageLen + KabuCodec::kHeaderLen)
	{
		int len = 0;
		evbuffer_copyout(evbuf, &len, sizeof(int));
		if (len > KabuCodec::kMaxMessageLen || len < KabuCodec::kMinMessageLen)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, codec->on_error_cb);
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, KabuCodec::kInvalidLength);
				lua_pcall(L, 1, 0, 0);
			}
			break;
		}
		else if (evbuffer_get_length(evbuf) >= (size_t)(len + KabuCodec::kHeaderLen))
		{
			KabuCodec::ErrorCode errorCode = KabuCodec::kNoError;
			char * data = (char*)evbuffer_pullup(evbuf, len + KabuCodec::kHeaderLen);
			google::protobuf::Message* message = KabuCodec::parse(data + KabuCodec::kHeaderLen, len, &errorCode);
			if (errorCode == KabuCodec::kNoError && message)
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, codec->on_data_cb);
				if (lua_isfunction(L, -1))
				{
					lua_pushlightuserdata(L, luabufferevent);
					LuaPB::pushMessage(L, message);
					lua_pcall(L, 2, 0, 0);
				}

		        evbuffer_drain(evbuf, len + KabuCodec::kHeaderLen);
			}
			else
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, codec->on_error_cb);
				if (lua_isfunction(L, -1))
				{
					lua_pushnumber(L, errorCode);
					lua_pcall(L, 1, 0, 0);
				}
				break;
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}

static int luacodec_send(lua_State* L)
{
	//luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);

	lua_bufferevent* luabufferevent = (lua_bufferevent*)(lua_touserdata(L, 1));
	LuaMessage* luamsg = (LuaMessage*)luaL_checkudata(L, 2, LuaPB::sMessageMeta);

	google::protobuf::Message* message = luamsg->msg;
	struct bufferevent* bufferevent = luabufferevent->bufferevent;
	struct evbuffer* evbuf = bufferevent_get_output(bufferevent);

	const std::string& typeName = message->GetTypeName();
	short nameLen = (short)(typeName.size());
	short headlen = 0;
	int byte_size = message->ByteSize();

	int totalLen = nameLen + sizeof(nameLen) + headlen + sizeof(headlen) + byte_size;
	evbuffer_add(evbuf, &totalLen, sizeof(int));
	evbuffer_add(evbuf, &nameLen, sizeof(short));
	evbuffer_add(evbuf, typeName.c_str(), nameLen);
	evbuffer_add(evbuf, &headlen, sizeof(short));

	struct evbuffer_iovec v[1];
	int n = evbuffer_reserve_space(evbuf, byte_size, v, 1);
	if (n<=0)
	{
		fprintf(stderr, "Unable to reserve the space for some reason. ");
		evbuffer_drain(evbuf, evbuffer_get_length(evbuf));
		return 0;
	}

	message->SerializeWithCachedSizesToArray((google::protobuf::uint8*)v[0].iov_base);
	v[0].iov_len = byte_size;

	if (evbuffer_commit_space(evbuf, v, 1) < 0)
	{
		fprintf(stderr, "Error committing. ");
		evbuffer_drain(evbuf, evbuffer_get_length(evbuf));
		return 0;
	}
	return 0;
}

static const luaL_Reg lib[] =
{
		{"new", luacodec_new},
		{"handle", luacodec_handle},
		{"send", luacodec_send},
		{NULL, NULL}
};

static const luaL_Reg libm[] =
{
		{"__gc", luacodec_free},
		{NULL, NULL}
};

int luaopen_luacodec(lua_State* L)
{
	luaL_newmetatable(L, LUA_CODEC_META);
	luaL_register(L, NULL, libm);

	luaL_register(L, LUA_CODEC, lib);

	LuaPB::openlib(L);
	lua_pop(L, 1);

	return 1;
}
