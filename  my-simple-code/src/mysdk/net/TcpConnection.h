
#ifndef MYSDK_NET_TCPCONNECTION_H
#define MYSDK_NET_TCPCONNECTION_H

#include <mysdk/base/Array.h>
#include <mysdk/base/Common.h>

#include <mysdk/net/Buffer.h>
#include <mysdk/net/Session.h>
#include <mysdk/net/Socket.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/Callbacks.h>

#include <string>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Socket;
	class TcpConnection : public array_item_t
	{
	public:
		TcpConnection(EventLoop* loop,
				const std::string& name,
				int sockfd,
				const InetAddress& localAddr,
				const InetAddress& peerAddr);
		~TcpConnection();

		EventLoop* getLoop() const { return loop_; }
		const std::string& name() const { return name_; }
		const InetAddress& localAddress() { return localAddr_; }
		const InetAddress& peerAddress() { return peerAddr_; }
		bool connected() const { return state_ == kConnected; }

		void send(Buffer* buf);
		void send(const void* data, size_t len);
		void send(const void* data);
		//void send(Buffer* message);
		void shutdown();
		void setTcpNoDelay(bool on);

		void setContext(void* context)
		{ context_ = context; }
		void* getContext()
		{ return context_; }
		const void* getContext() const
		{ return context_; }

		void setRecover()
		{
			recover_ = true;
		}

		bool getRecover()
		{
			return recover_;
		}

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb;}
		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ writeCompleteCallback_ = cb; }

		/// Internal use only.
		void setCloseCallback(const CloseCallback& cb)
		{ closeCallback_ = cb; }

		void connectEstablished();
		void connectDestroyed();

	private:
		enum StateE
		{
			kDisconnected,
			kConnecting,
			kConnected,
			kDisconnecting,
		};

		void handleRead(Timestamp receiveTime);
		void handleWrite();
		void handleClose();
		void handleError();

		void setState(StateE s) {state_ = s; }

		EventLoop* loop_;
		std::string name_;
		StateE state_;
		Socket socket_;
		Session session_;
		InetAddress localAddr_;
		InetAddress peerAddr_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		ConnectionCallback closeCallback_;

		Buffer inputBuffer_;
		Buffer outputBuffer_;

		bool recover_; //是否回收这个TcpConnection资源

		void* context_;
	private:
		DISALLOW_COPY_AND_ASSIGN(TcpConnection);
	};
}
}

#endif
