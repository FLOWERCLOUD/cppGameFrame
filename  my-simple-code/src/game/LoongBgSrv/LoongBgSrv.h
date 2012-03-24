
#ifndef GAME_LOONGBGSRV_H_
#define GAME_LOONGBGSRV_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>
#include <mysdk/net/TcpConnection.h>

#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

using namespace mysdk;
using namespace mysdk::net;

// 龙族对抗战场服务器
class LoongBgSrv
{
public:
	LoongBgSrv(EventLoop* loop, InetAddress& serverAddr);
	~LoongBgSrv();

public:
	void start();
	void stop();

	void onConnectionCallback(mysdk::net::TcpConnection* conn);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);

	void send(mysdk::net::TcpConnection* pCon, PacketBase& pb);
private:
	bool login(mysdk::net::TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);
	void tickMe();
private:
	EventLoop* loop_;
	TcpServer server_;
	KaBuCodec codec_;

private:
	DISALLOW_COPY_AND_ASSIGN(LoongBgSrv);
};

#endif /* LOONGBGSRV_H_ */
