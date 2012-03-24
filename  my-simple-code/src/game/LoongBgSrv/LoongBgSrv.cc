
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/BgPlayer.h>

#include <string.h>

LoongBgSrv::LoongBgSrv(EventLoop* loop, InetAddress& serverAddr):
	loop_(loop),
	server_(loop, serverAddr, "LoongBgSrv"),
	codec_(
				std::tr1::bind(&LoongBgSrv::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	server_.setConnectionCallback(std::tr1::bind(&LoongBgSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
														&codec_,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));

    loop->runEvery(0.025, std::tr1::bind(&LoongBgSrv::tickMe, this));
}

LoongBgSrv::~LoongBgSrv()
{
}

void LoongBgSrv::start()
{
	server_.start();
}

void LoongBgSrv::stop()
{
}

void LoongBgSrv::onConnectionCallback(mysdk::net::TcpConnection* conn)
{
	if (conn)
	{
		LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
				<< conn->localAddress().toHostPort() << " is"
				<< (conn->connected() ? "UP" : "DOWN");
	}
}

void LoongBgSrv::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 op = pb.getOP();
	if (op == game::LOGIN)
	{
		login(pCon, pb, timestamp);
	}
	else
	{
		BgPlayer* player = static_cast<BgPlayer*>(pCon->getContext());
		if (player != NULL)
		{
			player->onMsgHandler(pb);
		}
	}
}

bool LoongBgSrv::login(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	int32 playerId = pb.getInt32();
	char playerName[100];
	if (pb.getUTF(playerName, 99))
	{
		return false;
	}

	char md5[100];
	if (pb.getUTF(md5, 99))
	{
		return false;
	}

	BgPlayer* player = new BgPlayer(playerId, playerName, pCon, this);
	pCon->setContext(player);
	//  告诉客户端 登陆成功哦
	PacketBase op(client::LOGIN, 0);
	op.putInt32(0); // 登陆成功
	send(pCon, pb);
	return true;
}

void LoongBgSrv::tickMe()
{
	LOG_TRACE << "LoongBgSrv::tickMe - "  << Timestamp::now().toString();
}

void LoongBgSrv::send(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	codec_.send(pCon, pb);
}
