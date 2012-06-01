/*
 * LoongBgClient.h
 *
 *  Created on: 2012-4-19
 *    
 */

#ifndef GAME_LOONGBGCLIENT_H_
#define GAME_LOONGBGCLIENT_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/TcpClient.h>
#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

#include <string>

using namespace mysdk;
using namespace mysdk::net;

class Client
{
public:
	Client(EventLoop* loop, const InetAddress& listenAddr);
	~Client(){}

	void connect();
private:
	void onConnection(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp);

private:
	void sendPacket(PacketBase& pb);
	void onHandler();
private:
	EventLoop* loop_;
	TcpClient client_;
	mysdk::net::TcpConnection* pCon_;
	KaBuCodec codec_;
private:
	DISALLOW_COPY_AND_ASSIGN(Client);
};

#endif /* LOONGBGCLIENT_H_ */
