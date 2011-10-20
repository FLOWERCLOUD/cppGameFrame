
#include <mysdk/net/Acceptor.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/SocketsOps.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace mysdk;
using namespace mysdk::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr):
		loop_(loop),
		acceptSocket_(sockets::createNonblockingOrDie()),
		acceptSession_(loop, acceptSocket_.fd()),
		listenning_(false),
		idleFd_(::open("/dev/null", O_RDONLY))
{
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.bindAddress(listenAddr);
	acceptSession_.setReadCallback(std::tr1::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	::close(idleFd_);
}

void Acceptor::listen()
{
	listenning_ = true;
	acceptSocket_.listen();
	acceptSession_.enableReading();
}

void Acceptor::handleRead()
{
	InetAddress peerAddr(0);
	//while(true)
	{
			int connfd = acceptSocket_.accept(&peerAddr);
			if (connfd >= 0)
			{
				if (newConnectionCallback_)
				{
					newConnectionCallback_(connfd, peerAddr);
				}
				else
				{
					sockets::close(connfd);
				}
			}
			else
			{
				// Read the section named "The special problem of
				// accept()ing when you can't" in libev's doc.
				// By Marc Lehmann, author of livev.
				if (errno == EMFILE)
				{
					::close(idleFd_);
					idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
					::close(idleFd_);
					idleFd_ = ::open("/dev/null", O_RDONLY);
				}
				//break;
			}
	}
}
