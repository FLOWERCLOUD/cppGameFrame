
#include <mysdk/net/TcpConnection.h>

#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/SocketsOps.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>


namespace mysdk
{
namespace net
{
	void defaultConnectionCallback(TcpConnection* pConn)
	{
		assert(pConn != NULL);
		LOG_TRACE << pConn->localAddress().toHostPort() << "->"
				<< pConn->peerAddress().toHostPort() << " is "
				<< (pConn->connected() ? "UP" : "DOWN");
	}

	void defaultMessageCallback(TcpConnection* pConn, Buffer* buf, Timestamp timestamp)
	{
		assert(pConn != NULL && buf != NULL);
		buf->retrieveAll();
		return;
	}
}
}

using namespace mysdk;
using namespace mysdk::net;

TcpConnection::TcpConnection(EventLoop* loop,
								const std::string& conName,
								int sockfd,
								const InetAddress& localAddr,
								const InetAddress& peerAddr):
		loop_(loop),
		name_(conName),
		state_(kConnecting),
		socket_(sockfd),
		session_(loop, sockfd),
		localAddr_(localAddr),
		peerAddr_(peerAddr),
		recover_(false),
		context_(NULL)
{
	session_.setReadCallback(std::tr1::bind(&TcpConnection::handleRead, this, std::tr1::placeholders::_1));
	session_.setWriteCallback(std::tr1::bind(&TcpConnection::handleWrite, this));
	session_.setCloseCallback(std::tr1::bind(&TcpConnection::handleClose, this));
	session_.setErrorCallback(std::tr1::bind(&TcpConnection::handleError, this));
	LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this << " fd=" << sockfd;
}

TcpConnection::TcpConnection(EventLoop* loop,
		const std::string& conName,
		int sockfd,
		const InetAddress& localAddr,
		const InetAddress& peerAddr,
		const ConnectionCallback& concb,
		const MessageCallback& msgcb,
		const WriteCompleteCallback& writeComcb,
		const CloseCallback& closecb):
			loop_(loop),
			name_(conName),
			state_(kConnecting),
			socket_(sockfd),
			session_(loop, sockfd),
			localAddr_(localAddr),
			peerAddr_(peerAddr),
			connectionCallback_(concb),
			messageCallback_(msgcb),
			writeCompleteCallback_(writeComcb),
			closeCallback_(closecb),
			recover_(false),
			context_(NULL)
{
	session_.setReadCallback(std::tr1::bind(&TcpConnection::handleRead, this, std::tr1::placeholders::_1));
	session_.setWriteCallback(std::tr1::bind(&TcpConnection::handleWrite, this));
	session_.setCloseCallback(std::tr1::bind(&TcpConnection::handleClose, this));
	session_.setErrorCallback(std::tr1::bind(&TcpConnection::handleError, this));
	LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this << " fd=" << sockfd;
}

TcpConnection::~TcpConnection()
{
	 LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this << " fd=" << session_.fd();
}

void TcpConnection::send(const Buffer* buf)
{
  if (state_ == kConnected)
  {
	  send(const_cast<Buffer* >(buf)->peek(), const_cast<Buffer* >(buf)->readableBytes());
	  //const_cast<Buffer* >(buf)->retrieveAll();
  }
}

void TcpConnection::send(const void* data)
{
	send(data, strlen(static_cast<const char*>(data)));
}

void TcpConnection::send(const void* data, size_t len)
{
	//LOG_DEBUG << "TcpConnection::send "  << this ;
	if (state_ != kConnected) return;
	//LOG_DEBUG << "TcpConnection::send "  << this << " fd=" << session_.fd() << " len=" << len;
	ssize_t nwrote = 0;
	if (!session_.isWriting() && outputBuffer_.readableBytes() == 0)
	{
		nwrote = ::write(session_.fd(), data, len);
		//LOG_DEBUG << "========TcpConnection::send "  << this << " fd=" << session_.fd() << " len=" << len
		//						<< " data: " << data;
		if (nwrote >= 0)
		{
			if (implicit_cast<size_t>(nwrote) < len)
			{
				LOG_TRACE << "I am going to write more data";
			}
			else if (writeCompleteCallback_)
			{
				writeCompleteCallback_(this);
			}
		}
		else // nwrote < 0
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "TcpConnection::send";
			}
		}
	}

	assert(nwrote >= 0);
	if (implicit_cast<size_t>(nwrote) < len)
	{
		outputBuffer_.append(static_cast<const char*>(data) + nwrote, len - nwrote);
		if (!session_.isWriting())
		{
			session_.enableWriting();
		}
	}
}

void TcpConnection::shutdown()
{
	if (state_ == kConnected)
	{
		setState(kDisconnecting);
		if (!session_.isWriting())
		{
			// we are not writing
			socket_.shutdownWrite();
		}
	}
}

void TcpConnection::setTcpNoDelay(bool on)
{
	socket_.setTcpNoDelay(on);
}

void TcpConnection::connectEstablished()
{
	assert(state_ == kConnecting);
	setState(kConnected);
	session_.enableReading();
	connectionCallback_(this);
}

void TcpConnection::connectDestroyed()
{
	/*
	if (state_ == kConnected  || state_ == kDisconnecting)
	{
		setState(kDisconnected);
		session_.disableAll();

		connectionCallback_(this);
	}
	*/

	setState(kDisconnected);
	connectionCallback_(this);

	LOG_TRACE << "TcpConnection::connectDestroyed  " << name_;
	// must be the last line
	closeCallback_(this);
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	LOG_DEBUG << "TcpConnection::handleRead "  << this << " fd=" << session_.fd();

	int savedErrno;
	ssize_t n = inputBuffer_.readFd(session_.fd(), &savedErrno);
	LOG_DEBUG << "TcpConnection::handleRead "  << this << " fd=" << session_.fd() << " n:" << n;
	if (n > 0)
	{
		messageCallback_(this, &inputBuffer_, receiveTime);
		if (getRecover())
		{
			handleClose();
		}
	}
	else if (n == 0)
	{
		handleClose();
	}
}

void TcpConnection::handleWrite()
{
	if (!session_.isWriting())
	{
		 LOG_TRACE << "Connection is down, no more writing";
		 return;
	}

	ssize_t n = ::write(session_.fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
	if (n > 0)
	{
		outputBuffer_.retrieve(n);
		if (outputBuffer_.readableBytes() == 0)
		{
			session_.disableWriting();
			if (writeCompleteCallback_)
			{
				writeCompleteCallback_(this);
			}
			if (state_ == kDisconnecting)
			{
				shutdown();
			}
		}
		else
		{
			 LOG_TRACE << "I am going to write more data";
		}
	}
	else
	{
	      LOG_SYSERR << "TcpConnection::handleWrite";
	      abort();  // FIXME
	}
}

void TcpConnection::handleClose()
{
	  LOG_TRACE << "TcpConnection::handleClose state = " << state_;

	  if (state_ == kConnected  || state_ == kDisconnecting)
	  {
			setState(kDisconnected);
			session_.disableAll();

			//connectionCallback_(this);
	  }

	  loop_->queueInLoop(std::tr1::bind(&TcpConnection::connectDestroyed, this));
}

void TcpConnection::handleError()
{
	  int err = sockets::getSocketError(session_.fd());
	  LOG_ERROR << "TcpConnection::handleError [" << name_
	            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}
