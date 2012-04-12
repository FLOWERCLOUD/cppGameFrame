
#include <mysdk/net/TcpClient.h>

#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/SocketsOps.h>

#include <stdio.h>  // snprintf


using namespace mysdk;
using namespace mysdk::net;

TcpClient::TcpClient(EventLoop* loop,
		const InetAddress& serverAddr,
		const std::string& name):
		loop_(loop),
		connector_(loop, serverAddr),
		name_(name),
		connectionCallback_(defaultConnectionCallback),
		messageCallback_(defaultMessageCallback),
		retry_(false),
		connect_(true),
		nextConnId_(1),
		pConnection_(NULL)
{
	connector_.setNewConnectionCallback(std::tr1::bind(
			&TcpClient::newConnection,
			this,
			std::tr1::placeholders::_1));
}

TcpClient::~TcpClient()
{
	LOG_TRACE << "TcpClient::~TcpClient[" << name_ << "] - close "
			<< connector_.serverAddress().toHostPort();

	disconnect();
}

void TcpClient::connect()
{
	LOG_INFO << "TcpClient::connection[" << name_ << "] - connecting to "
			<< connector_.serverAddress().toHostPort();
	connect_ = true;
	connector_.start();
}

void TcpClient::disconnect()
{
	connect_ = false;
	if (pConnection_)
	{
		pConnection_->shutdown();
		pConnection_ = NULL;
	}
}

void TcpClient::newConnection(int sockfd)
{
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof(buf) - 1, ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	pConnection_ = new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr);
	assert(pConnection_ != NULL);
	pConnection_->setConnectionCallback(connectionCallback_);
	pConnection_->setMessageCallback(messageCallback_);
	pConnection_->setWriteCompleteCallback(writeCompleteCallback_);
	pConnection_->setCloseCallback(std::tr1::bind(&TcpClient::removeConnection, this, std::tr1::placeholders::_1));
	pConnection_->connectEstablished();
}

void TcpClient::removeConnection(TcpConnection* conn)
{
	assert(conn != NULL);
	if (retry_ && connect_)
	{
	    LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
	             << connector_.serverAddress().toHostPort();
	    connector_.restart();
	}

	delete conn;
	pConnection_ = NULL;
}

