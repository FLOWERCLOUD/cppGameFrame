
#ifndef GAME_LOONGBGCLIENT_H_
#define GAME_LOONGBGCLIENT_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/TcpClient.h>

#include <game/dbsrv/codec/codec.h>
#include <string>

using namespace mysdk;
using namespace mysdk::net;

class DBClient
{
public:
	DBClient(EventLoop* loop, const InetAddress& listenAddr);
	~DBClient(){}

	void connect();

public:
	void sendGetCmd();
	void sendSetCmd();
	void sendUnknowCmd();
private:
	void onConnection(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon,
			google::protobuf::Message* msg,
			mysdk::Timestamp timestamp);

private:
	void send(google::protobuf::Message* message);
private:
	EventLoop* loop_;
	TcpClient client_;
	mysdk::net::TcpConnection* pCon_;
	KabuCodec codec_;
};

#endif /* LOONGBGCLIENT_H_ */
