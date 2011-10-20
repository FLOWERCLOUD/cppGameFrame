#include <mysdk/net/TcpServer.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#include <tr1/functional>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace mysdk;
using namespace mysdk::net;

class EchoServer
{
 public:
  EchoServer(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "EchoServer")
  {
    server_.setConnectionCallback(std::tr1::bind(
    		&EchoServer::onConnection,
    		this,
    		std::tr1::placeholders::_1));

    server_.setMessageCallback(std::tr1::bind(&EchoServer::onMessage,
    		this,
    		std::tr1::placeholders::_1,
    		std::tr1::placeholders::_2,
    		std::tr1::placeholders::_3));
  }

  void start()
  {
    server_.start();
  }

 private:
  void onConnection(TcpConnection* conn)
  {
    LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
        << conn->localAddress().toHostPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");

    //conn->send("hello\n");
    const char* sendBuf = "world\n";
    conn->send(sendBuf, strlen(sendBuf));
  }

  void onMessage(TcpConnection* conn, Buffer* buf, Timestamp time)
  {
    std::string msg(buf->retrieveAsString());
    LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
    LOG_TRACE << "msg[ " << msg << " ]";
    if (msg == "exit\r\n")
    {
    	LOG_TRACE << "msg " << " ";
      conn->send("bye\n");
      conn->shutdown();
    }
    if (msg == "quit\r\n")
    {
    	LOG_TRACE << "quit"  << " ";
      loop_->quit();
    }
    conn->send(msg.c_str());
    return;
  }

  EventLoop* loop_;
  TcpServer server_;
};

int main(int argc, char* argv[])
{
	  EventLoop loop;
	  InetAddress listenAddr(2000);
	  EchoServer server(&loop, listenAddr);

	  server.start();

	  loop.loop();
}

