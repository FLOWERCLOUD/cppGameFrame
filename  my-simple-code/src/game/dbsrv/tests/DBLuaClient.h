
#ifndef GAME_DBLUACLIENT_H_
#define GAME_DBLUACLIENT_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/TcpClient.h>

#include <game/dbsrv/codec/codec.h>
#include <game/dbsrv/lua/LuaEngine.h>

#include <string>

class DBLuaClient
{
public:
	DBLuaClient(mysdk::net::EventLoop* loop, const mysdk::net::InetAddress& listenAddr);
	~DBLuaClient();

	void connect();

	void send(mysdk::net::TcpConnection* pCon, google::protobuf::Message* message);
private:
	void onConnection(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon,
			google::protobuf::Message* msg,
			mysdk::Timestamp timestamp);
private:
	void init();
private:
	void onLuaConnection(mysdk::net::TcpConnection* pCon);
	void onLuaMessage(mysdk::net::TcpConnection* pCon,
			google::protobuf::Message* msg,
			mysdk::Timestamp timestamp);
	void onLuaDisConnection(mysdk::net::TcpConnection* pCon);
private:
	mysdk::net::EventLoop* loop_;
	mysdk::net::TcpClient client_;
	KabuCodec codec_;
	LuaEngine luaEngine_;
};

#endif /* LOONGBGCLIENT_H_ */
