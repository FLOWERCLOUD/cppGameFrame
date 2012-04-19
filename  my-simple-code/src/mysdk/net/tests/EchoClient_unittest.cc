#include <mysdk/net/TcpClient.h>

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#include <utility>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace mysdk;
using namespace mysdk::net;

class EchoClient;
std::vector<EchoClient*> clients;
int current = 0;

class EchoClient
{
public:
	EchoClient(EventLoop* loop, const InetAddress& listenAddr, const string& id):
		loop_(loop),
		client_(loop, listenAddr, "EchoClient" + id)
	{
		client_.setConnectionCallback(std::tr1::bind(&EchoClient::onConnection, this, std::tr1::placeholders::_1));
		client_.setMessageCallback(std::tr1::bind(
				&EchoClient::onMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3));
	}

	void connect()
	{
		client_.enableRetry();
		client_.connect();
	}

private:
	void onConnection(TcpConnection* conn)
	{
	    LOG_TRACE << conn->localAddress().toHostPort() << " -> "
	        << conn->peerAddress().toHostPort() << " is "
	        << (conn->connected() ? "UP" : "DOWN");

	    if (conn->connected())
	    {
	      ++current;
	      if (implicit_cast<size_t>(current) < clients.size())
	      {
	        clients[current]->connect();
	      }
	      LOG_INFO << "*** connected " << current;
	    }
	    const char* sendBuf = "world\n";
	    conn->send(sendBuf, strlen(sendBuf));
	}

	void onMessage(TcpConnection* conn, Buffer* buf, Timestamp time)
	{
	    string msg(buf->retrieveAsString());
	    LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
	    return;
	}

	EventLoop* loop_;
	TcpClient client_;
 private:
	DISALLOW_COPY_AND_ASSIGN(EchoClient);
};

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		EventLoop loop;
		InetAddress serverAddr(argv[1], 2000);

		int n = 1;
		if ( argc > 2)
		{
			n = atoi(argv[2]);
		}
		clients.reserve(n);
		for (int i = 0; i < n; ++i)
		{
			char buf[32];
			snprintf(buf, sizeof(buf), "%d", i + 1);
			clients.push_back(new EchoClient(&loop, serverAddr, buf));
		}

		clients[current]->connect();
		loop.loop();
		while (!clients.empty() )
		{
			delete clients.back();
			clients.pop_back();
		}
	}
}
