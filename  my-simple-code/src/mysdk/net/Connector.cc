
#include <mysdk/net/Connector.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/Session.h>
#include <mysdk/net/SocketsOps.h>
#include <mysdk/base/Timestamp.h>

#include <errno.h>

#include <stdio.h>

using namespace mysdk;
using namespace mysdk::net;

const int Connector::kMaxRetryDelayMs = 30 * 1000;
const int Connector::kInitRetryDelayMs = 500;

Connector::Connector(EventLoop* loop, const InetAddress& addr):
		loop_(loop),
		serverAddr_(addr),
		state_(kDisconnected),
		retryDelayMs_(kInitRetryDelayMs)
{
		pSession_ = NULL;
}

Connector::~Connector()
{
	if (pSession_)
	{
		delete pSession_;
	}
}

void Connector::start()
{
	assert(state_ == kDisconnected);
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
	int savedErrno = ( ret == 0) ? 0 : errno;
	switch (savedErrno)
	{
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
		  connecting(sockfd);
		  break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
		  retry(sockfd);
		  break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
		  LOG_SYSERR << "connect error in Connector::start " << savedErrno;
		  sockets::close(sockfd);
		  break;

		default:
		  LOG_SYSERR << "Unexpected error in Connector::start " << savedErrno;
		  sockets::close(sockfd);
		  break;
	}
}

void Connector::restart()
{
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	start();
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	if (pSession_)
	{
		delete pSession_;
	}
	pSession_ = new Session(loop_, sockfd);
	assert(pSession_ != NULL);
	pSession_->setWriteCallback(std::tr1::bind(&Connector::handleWrite, this));
	pSession_->enableWriting();
}

int Connector::removeAddResetSession()
{
	assert(pSession_ != NULL);

	pSession_->disableAll();
	int sockfd = pSession_->fd();

	loop_->queueInLoop(std::tr1::bind(&Connector::DestroySession, this));
	return sockfd;
}

void Connector::DestroySession()
{
	if (pSession_)
	{
		delete pSession_;
		pSession_ = NULL;
	}
}

void Connector::handleWrite()
{
	  LOG_TRACE << "Connector::handleWrite";
	  if (state_ == kConnecting)
	  {
		  int sockfd = removeAddResetSession();
		  int err = sockets::getSocketError(sockfd);
		  if (err)
		  {
		      LOG_WARN << "Connector::handleWrite - SO_ERROR = "
		               << err << " " << strerror_tl(err);
		      retry(sockfd);
		  }
		  else if (sockets::isSelfConnect(sockfd))
		  {
		      LOG_WARN << "Connector::handleWrite - Self connect";
		      retry(sockfd);
		  }
		  else
		  {
		      setState(kConnected);
		      newConnectionCallback_(sockfd);
		  }

		  if (pSession_)
		  {
				//delete pSession_;
				//pSession_ = NULL;
		  }
	  }
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);

	LOG_INFO << "Connector::retry - Retry connecting to " << serverAddr_.toHostPort()
	           << " in " << retryDelayMs_ << " milliseconds. ";
	loop_->runAfter(retryDelayMs_ / 1000.0, std::tr1::bind(&Connector::start, this));
	retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
}
