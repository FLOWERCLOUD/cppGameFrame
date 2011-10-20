
#ifndef MYSDK_NET_ACCEPTOR_H
#define MYSDK_NET_ACCEPTOR_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Session.h>
#include <mysdk/net/Socket.h>

#include <tr1/functional>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class InetAddress;
	class Acceptor
	{
	public:
		typedef std::tr1::function<void (int sockfd, const InetAddress& )> NewConnectionCallback;

		Acceptor(EventLoop* loop, const InetAddress& listenAddr);
		~Acceptor();

		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{ newConnectionCallback_ = cb; }
		bool listenning() const { return listenning_; }
		void listen();

	private:
		void handleRead();

		EventLoop* loop_;
		Socket acceptSocket_;
		Session acceptSession_;

		NewConnectionCallback newConnectionCallback_;

		bool listenning_;
		int idleFd_;
	private:
		DISALLOW_COPY_AND_ASSIGN(Acceptor);
	};
}
}

#endif
