/*
 * LogSrv.cc
 *
 *  Created on: 2012-4-17
 *    
 */

#include <game/LogSrv/LogSrv.h>

LogSrv::LogSrv(EventLoop* loop, InetAddress& serverAddr):
	loop_(loop),
	server_(loop, serverAddr, "LogSrv")
{
	server_.setConnectionCallback(std::tr1::bind(&LogSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&LogSrv::onMessage,
														this,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));
}

LogSrv::~LogSrv()
{

}

void LogSrv::start()
{
	server_.start();
}

void LogSrv::stop()
{
}


void LogSrv::onConnectionCallback(mysdk::net::TcpConnection* pCon)
{
	if (pCon)
	{
		LOG_TRACE << pCon->peerAddress().toHostPort() << " -> "
				<< pCon->localAddress().toHostPort() << " is"
				<< (pCon->connected() ? "UP" : "DOWN");
		// 玩家断开了连接
		if (!pCon->connected())
		{
		}
	}
}

void LogSrv::onMessage(mysdk::net::TcpConnection* pCon,
								mysdk::net::Buffer* pBuf,
								mysdk::Timestamp receiveTime)
{

}

