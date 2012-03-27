
#include <mysdk/net/TcpServer.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/SocketsOps.h>
#include <mysdk/net/TcpConnection.h>

#include <stdio.h>  // snprintf

using namespace mysdk;
using namespace mysdk::net;

TcpServer::TcpServer(EventLoop* loop,
						const InetAddress& listenAddr,
						const std::string& serverName):
		loop_(loop),
		hostport_(listenAddr.toHostPort()),
		name_(serverName),
		acceptor_(loop, listenAddr),
		connectionCallback_(defaultConnectionCallback),
		messageCallback_(defaultMessageCallback),
		started_(false),
		nextConnId_(1)
{
	acceptor_.setNewConnectionCallback(std::tr1::bind(
			&TcpServer::newConnection,
			this,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2));
}

TcpServer::~TcpServer()
{
	  LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

	  while (!tcpConnectionArray_.empty())
	  {
		  TcpConnectionArray::size_type size = tcpConnectionArray_.size();
		  TcpConnectionArray::size_type index = size - 1;
		  TcpConnection* conn = tcpConnectionArray_[index];
		  if (conn)
		  {
			  conn->connectDestroyed();
		  }
		  //tcpConnectionArray_.erase(index);
		  //delete conn;
	  }
	  tcpConnectionArray_.clear();
}

void TcpServer::start()
{
	started_ = true;
	if (!acceptor_.listenning())
	{
		acceptor_.listen();
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	char buf[32];
	snprintf(buf, sizeof(buf) - 1, ":%s#%d", hostport_.c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	LOG_INFO << "TcpServer::newConnection [" << name_
	           << "] - new connection [" << connName
	           << "] from " << peerAddr.toHostPort();

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
#if 0
	TcpConnection* pConn = new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr);
	assert(pConn != NULL);
	tcpConnectionArray_.push_back(pConn);
	LOG_TRACE << "TcpServer::newConnection tcpConnectionArray size[" << tcpConnectionArray_.size() << "]";
	pConn->setConnectionCallback(connectionCallback_);
	pConn->setMessageCallback(messageCallback_);
	pConn->setWriteCompleteCallback(writeCompleteCallback_);
	pConn->setCloseCallback(std::tr1::bind(&TcpServer::removeConnection, this, std::tr1::placeholders::_1));
	pConn->connectEstablished();
#endif
	TcpConnection* pConn = new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr,
															connectionCallback_,
															messageCallback_,
															writeCompleteCallback_,
															std::tr1::bind(&TcpServer::removeConnection, this, std::tr1::placeholders::_1));
	assert(pConn != NULL);
	tcpConnectionArray_.push_back(pConn);
	LOG_DEBUG << "TcpServer::newConnection tcpConnectionArray size[" << tcpConnectionArray_.size() << "]";
	pConn->connectEstablished();
}

void TcpServer::removeConnection(TcpConnection* conn)
{
	assert(conn != NULL);
	tcpConnectionArray_.erase(conn);
	LOG_TRACE << "tcpConnectionArray size[" << tcpConnectionArray_.size() << "]";
	delete conn;
}
