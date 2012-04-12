/*
 * Hotel.cc
 *
 *  Created on: 2012-4-12
 *    
 */

#include <game/LoongBgSrv/Hotel.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>

Hotel::Hotel(EventLoop* loop, const InetAddress& listenAddr) :
	pCon_(NULL),
	loop_(loop),
    client_(loop, listenAddr, "Hotel"),
    codec_(
  				std::tr1::bind(&Hotel::onKaBuMessage,
  				this,
  				std::tr1::placeholders::_1,
  				std::tr1::placeholders::_2,
  				std::tr1::placeholders::_3))
{
		client_.setConnectionCallback(
				std::tr1::bind(&Hotel::onConnection, this, std::tr1::placeholders::_1));

		client_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
				&codec_,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3));

		client_.enableRetry();

		LOG_TRACE << "Hotel::Hotel";
}

Hotel::~Hotel()
{
	LOG_TRACE << "Hotel::~Hotel";
}

void Hotel::send(PacketBase& pb)
{
	if (pCon_)
	{
		codec_.send(pCon_, pb);
	}
}

void Hotel::onKaBuMessage(TcpConnection* pCon, PacketBase& pb, Timestamp timestamp)
{
}

void Hotel::onConnection(TcpConnection* pCon)
{
	if (!pCon) return;
	LOG_INFO << pCon->localAddress().toHostPort() << " -> "
			<< pCon->peerAddress().toHostPort() << " is "
			<< (pCon->connected() ? "UP" : "DOWN");

	if (pCon->connected())
	{
		pCon_ = pCon;
	}
	else
	{
		pCon_ = NULL;
	}
}
