/*
 * LogSrv.h
 *
 *  Created on: 2012-4-17
 *    
 */

#ifndef GAME_LOGSRV_H_
#define GAME_LOGSRV_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>
#include <mysdk/net/TcpConnection.h>

using namespace mysdk;
using namespace mysdk::net;

class LogSrv
{
public:
	LogSrv(EventLoop* loop, InetAddress& serverAddr);
	~LogSrv();

public:
	void start();
	void stop();

	void onConnectionCallback(mysdk::net::TcpConnection* pCon);
	void onMessage(mysdk::net::TcpConnection* pCon,
									mysdk::net::Buffer* pBuf,
									mysdk::Timestamp receiveTime);

private:
	EventLoop* loop_;
	TcpServer server_;
private:
	DISALLOW_COPY_AND_ASSIGN(LogSrv);
};

#endif /* LOGSRV_H_ */
