
#ifndef MYSDK_NET_CONNECTOR_H
#define MYSDK_NET_CONNECTOR_H

#include <mysdk/base/Common.h>

#include <mysdk/net/InetAddress.h>

#include <tr1/functional>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Session;
	class Connector
	{
	public:
		typedef std::tr1::function <void (int sockfd)> NewConnectionCallback;

	public:
		Connector(EventLoop* loop, const InetAddress& serverAddr);
		~Connector();

		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{ newConnectionCallback_ = cb; }

		void start();
		void restart();

		const InetAddress& serverAddress() const { return serverAddr_; };
	private:
		enum States
		{
			kDisconnected,
			kConnecting,
			kConnected,
		};

		static const int kMaxRetryDelayMs;
		static const int kInitRetryDelayMs;

		void setState(States s) { state_ = s; }

		void connecting(int sockfd);
		void handleWrite();

		void retry(int sockfd);
		int removeAddResetSession();

		void DestroySession();

		EventLoop* loop_;
		InetAddress serverAddr_;
		States state_;
		Session* pSession_;
		NewConnectionCallback newConnectionCallback_;
		int retryDelayMs_;
	private:
		  DISALLOW_COPY_AND_ASSIGN(Connector);
	};
}
}

#endif  // MUDUO_NET_CONNECTOR_H
