/*
 * TestLoongBgSrv.cc
 *
 *  Created on: 2012-3-19
 *    
 */

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpClient.h>

#include <game/LoongBgSrv/LoongBgSrv.h>

#include <string.h>

static const int sTestTime = 3;

class TestLoongBgClient
{
 public:
	TestLoongBgClient(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(loop),
      client_(loop, listenAddr, "TestLoongBgClient"),
      codec_(
  				std::tr1::bind(&TestLoongBgClient::onKaBuMessage,
  				this,
  				std::tr1::placeholders::_1,
  				std::tr1::placeholders::_2,
  				std::tr1::placeholders::_3)),
  	 testTimes_(0)
	{
		client_.setConnectionCallback(
				std::tr1::bind(&TestLoongBgClient::onConnection, this, std::tr1::placeholders::_1));

		client_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
				&codec_,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3));
	}

	void connect()
	{
		client_.connect();
	}
private:
	void onConnection(mysdk::net::TcpConnection* pCon)
	{
		if (!pCon) return;

		LOG_INFO << pCon->localAddress().toHostPort() << " -> "
				<< pCon->peerAddress().toHostPort() << " is "
				<< (pCon->connected() ? "UP" : "DOWN");

		if (pCon->connected())
		{
			LOG_TRACE << "TEST ..... ";
			PacketBase pb;
			int32 be32 = 100;
			pb.putInt32(be32);
			pb.putUTF("hello world");
			codec_.send(pCon, pb);
		}
	}

	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
	{
		if (testTimes_++ <= sTestTime)
		{
			int32 be32 = pb.getInt32();
			char buf[100];
			pb.getUTF(buf, 100);
			LOG_TRACE << "========== be32[" << be32 <<"]["<< buf << "]" ;

			// send msg
			PacketBase op;
			op.putInt32(be32);
			op.putUTF(buf, static_cast<int16>(strlen(buf)));
			codec_.send(pCon, op);
		}
		else
		{
			LOG_TRACE << "========== be shutdown" ;
			pCon->shutdown();
		}
	}
private:
	  EventLoop* loop_;
	  TcpClient client_;
	  KaBuCodec codec_;
	  int32 testTimes_;
};


int main(int argc, char* argv[])
{
	  LOG_INFO << "pid = " << getpid();
	  if (argc > 1)
	  {
		  EventLoop loop;
		  InetAddress serverAddr(argv[1], 2007);

		  TestLoongBgClient client(&loop, serverAddr);
		  client.connect();
		  loop.loop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip\n", argv[0]);
	  }
	return 0;
}
