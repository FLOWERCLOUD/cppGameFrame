
#include <game/bosssrv/tests/Client.h>

#include <mysdk/base/Logging.h>

#include <game/bosssrv/protocol/GameProtocol.h>

Client::Client(EventLoop* loop, const InetAddress& listenAddr):
	loop_(loop),
	client_(loop, listenAddr, "Client"),
	pCon_(NULL),
	codec_(
				std::tr1::bind(&Client::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	client_.setConnectionCallback(
			std::tr1::bind(&Client::onConnection, this, std::tr1::placeholders::_1));

	client_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
			&codec_,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2,
			std::tr1::placeholders::_3));

	loop_->runEvery(1.0, std::tr1::bind(&Client::onHandler, this));
}

void Client::connect()
{
	client_.connect();
}

// return [nBegin, nEnd]
static inline int32 getRandomBetween(int32 nBegin, int32 nEnd)
{
	return nBegin + static_cast<int32>(random() % (nEnd - nBegin + 1) );
}

void Client::onHandler()
{
	int uid = getRandomBetween(1, 200000);
	char username[512];
	snprintf(username, 512, "Test_%d", uid);
	int hurtValue = getRandomBetween(1, 10);
	PacketBase pb(game::OP_BOSSSRV_UPDATEDATA, 0);
	pb.putInt32(uid);
	pb.putUTF(username);
	pb.putInt32(hurtValue);
	sendPacket(pb);
}

void Client::onConnection(mysdk::net::TcpConnection* pCon)
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

void Client::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 cmd = pb.getOP();
	if (cmd == game::OP_BOSSSRV_UPDATEDATA)
	{
		uint32 param = pb.getParam();
		if (param == 1)
		{
			uint32 bossId = pb.getInt32();
			uint32 bosshp = pb.getInt32();
			uint32 bosselem = pb.getInt32();

			LOG_INFO << " bossId: " << bossId
								<< " bosshp: " << bosshp
								<< " bosselem: " << bosselem;
		}
	}
}



void Client::sendPacket(PacketBase& pb)
{
	if (pCon_)
	{
		codec_.send(pCon_, pb);
	}
}


