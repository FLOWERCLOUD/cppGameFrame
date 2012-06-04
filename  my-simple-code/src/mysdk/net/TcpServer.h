
#ifndef MYSDK_NET_TCPSERVER_H
#define MYSDK_NET_TCPSERVER_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Acceptor.h>
#include <mysdk/net/Callbacks.h>
#include <mysdk/net/TcpConnection.h>

#include <string>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class InetAddress;
	class TcpServer
	{
	public:
		typedef array_t<TcpConnection> TcpConnectionArray;

	public:
		TcpServer(EventLoop* loop,
				const InetAddress& listenAddr,
				const std::string& name);
		~TcpServer();

		const std::string& hostport() const { return hostport_; }
		const std::string& name() const { return name_; }

		void start();

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }

		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ writeCompleteCallback_ = cb; }

		size_t getConnectionNum()
		{
			return tcpConnectionArray_.size();
		}
		TcpConnectionArray& getTcpConnectionArray()
		{
			return tcpConnectionArray_;
		}
	private:
		void newConnection(int sockfd, const InetAddress& peerAddr);
		void removeConnection(TcpConnection* conn);

	private:
		EventLoop* loop_;
		const std::string hostport_;
		const std::string name_;
		Acceptor acceptor_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;

		bool started_;
		int nextConnId_;
		TcpConnectionArray tcpConnectionArray_;
	private:
		DISALLOW_COPY_AND_ASSIGN(TcpServer);
	};
}
}

#endif
