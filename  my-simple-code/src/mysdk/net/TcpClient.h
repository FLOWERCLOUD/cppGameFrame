
#ifndef MYSDK_NET_TCPCLIENT_H
#define MYSDK_NET_TCPCLIENT_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Connector.h>
#include <mysdk/net/TcpConnection.h>

#include <string>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class TcpClient
	{
	public:
		TcpClient(EventLoop* loop,
				const InetAddress& serverAddr,
				const std::string& name);
		~TcpClient();

		void connect();
		void disconnect();

		bool retry() const;
		void enableRetry() { retry_ = true;}

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }

		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ writeCompleteCallback_ = cb; }

		TcpConnection* getConnection()
		{
			return pConnection_;
		}
	private:
		void newConnection(int sockfd);
		void removeConnection(TcpConnection* con);

		EventLoop* loop_;
		Connector connector_;
		const std::string name_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		bool retry_;
		bool connect_;
		int nextConnId_;
		TcpConnection* pConnection_;
	private:
		DISALLOW_COPY_AND_ASSIGN(TcpClient);
	};
}
}

#endif
