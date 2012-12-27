
#include "DBLuaClient.h"
#include "LuaClient.h"

#include <game/dbsrv/lua/LuaPB.h>

using namespace mysdk;
using namespace mysdk::net;

DBLuaClient::DBLuaClient(EventLoop* loop, const InetAddress& listenAddr):
	loop_(loop),
	client_(loop, listenAddr, "DBLuaClient"),
	codec_(
				std::tr1::bind(&DBLuaClient::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	client_.setConnectionCallback(
			std::tr1::bind(&DBLuaClient::onConnection, this, std::tr1::placeholders::_1));

	client_.setMessageCallback(std::tr1::bind(&KabuCodec::onMessage,
			&codec_,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2,
			std::tr1::placeholders::_3));

	init();
}

void DBLuaClient::init()
{
	// 开启lua引擎
	luaEngine_.openlibs();
	// 开启自定义的lib库
	lua_State *L = luaEngine_.getLuaState();
	LuaClient::openlib(L);
	LuaPB::openlib(L);

	lua_pushlightuserdata(L, this);
	lua_setglobal(L, "sClient");
	// 导入client.lua
	luaEngine_.dofile("client.lua");
}

DBLuaClient::~DBLuaClient()
{

}

void DBLuaClient::connect()
{
	client_.connect();
}

void DBLuaClient::onConnection(mysdk::net::TcpConnection* pCon)
{
	if (!pCon) return;
	LOG_INFO << pCon->localAddress().toHostPort() << " -> "
			<< pCon->peerAddress().toHostPort() << " is "
			<< (pCon->connected() ? "UP" : "DOWN");

	if (pCon->connected())
	{
		onLuaConnection(pCon);
	}
	else
	{
		onLuaDisConnection(pCon);
	}
}

void DBLuaClient::onKaBuMessage(mysdk::net::TcpConnection* pCon,
		google::protobuf::Message* message,
		mysdk::Timestamp timestamp)
{
	onLuaMessage(pCon, message, timestamp);
}

void DBLuaClient::send(mysdk::net::TcpConnection* pCon, google::protobuf::Message* message)
{
    codec_.send(pCon, message);
}

void DBLuaClient::onLuaConnection(mysdk::net::TcpConnection* pCon)
{
	lua_State *L = luaEngine_.getLuaState();
	lua_getglobal(L, "onConnection");
	if (!lua_isfunction(L, -1))
	{
		fprintf(stderr, "onConnection function not found!!!\n");
		return;
	}
	lua_pushlightuserdata(L, pCon);
	//LuaClient::pushTcpConnection(L, pCon);
	if (lua_pcall(L, 1, 1, 0) != 0)
	{
		fprintf(stderr, "call onConnection error, result: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (!lua_isnumber(L, -1))
	{
		fprintf(stderr, "call onConnection return result type error\n");
		return;
	}

	//int ret = static_cast<int>(lua_tonumber(L, -1));
	//printf("onConnection ret: %d\n", ret);
}

void DBLuaClient::onLuaMessage(mysdk::net::TcpConnection* pCon,
		google::protobuf::Message* msg,
		mysdk::Timestamp timestamp)
{
	lua_State *L = luaEngine_.getLuaState();
	lua_getglobal(L, "onMessage");
	if (!lua_isfunction(L, -1))
	{
		fprintf(stderr, "onMessage function not found!!!\n");
		return;
	}
	lua_pushlightuserdata(L, pCon);
	//LuaClient::pushTcpConnection(L, pCon);
	LuaPB::pushMessage(L, msg);
	if (lua_pcall(L, 2, 1, 0) != 0)
	{
		fprintf(stderr, "call onMessage error, result: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (!lua_isnumber(L, -1))
	{
		fprintf(stderr, "call onMessage return result type error\n");
		return;
	}

	//int ret = static_cast<int>(lua_tonumber(L, -1));
	//printf("onLuaMessage ret: %d\n", ret);
}

void DBLuaClient::onLuaDisConnection(mysdk::net::TcpConnection* pCon)
{
	lua_State *L = luaEngine_.getLuaState();
	lua_getglobal(L, "onDisConnection");
	if (!lua_isfunction(L, -1))
	{
		fprintf(stderr, "onDisConnection function not found!!!\n");
		return;
	}
	lua_pushlightuserdata(L, pCon);
	//LuaClient::pushTcpConnection(L, pCon);
	if (lua_pcall(L, 1, 1, 0) != 0)
	{
		fprintf(stderr, "call onDisConnection error, result: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (!lua_isnumber(L, -1))
	{
		fprintf(stderr, "call onDisConnection return result type error\n");
		return;
	}

	//int ret = static_cast<int>(lua_tonumber(L, -1));
	//printf("onLuaDisConnection ret: %d\n", ret);
}
