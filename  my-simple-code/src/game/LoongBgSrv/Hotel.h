/*
 * Hotel.h
 *
 *  Created on: 2012-4-12
 *    
 */

#ifndef GAME_HOTEL_H_
#define GAME_HOTEL_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpClient.h>
#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

using namespace mysdk;
using namespace mysdk::net;

class Hotel
{
public:
	Hotel(EventLoop* loop, const InetAddress& listenAddr);
	~Hotel();

	void send(PacketBase& pb);
private:
	void onConnection(TcpConnection* pCon);
	void onKaBuMessage(TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);

	TcpConnection* pCon_;
	EventLoop* loop_;
	TcpClient client_;
	KaBuCodec codec_;
};

#endif /* HOTEL_H_ */
