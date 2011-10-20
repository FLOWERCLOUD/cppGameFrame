
#ifndef MYSDK_NET_SESSION_H_
#define MYSDK_NET_SESSION_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <tr1/functional>
#include <string>

namespace mysdk
{
namespace net
{
	class EventLoop;
	class Session
	{
	public:
		typedef std::tr1::function<void(Timestamp)> ReadEventCallback;
		typedef std::tr1::function<void()> EventCallback;

		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;
		static const int kErrorEvent;
		static const int kHupEvent;

	public:
		Session(EventLoop* loop, int fd);
		~Session();

		void handleEvent(Timestamp receiveTime);

		void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
		void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
		void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }
		void setErrorCallback(const EventCallback& cb) {errorCallback_ = cb; }

		int fd() const { return fd_; }
		EventLoop* getLoop() { return loop_; }

		int events() const { return events_; }
		void setFireEvents(int fireEvents) { fireEvents_ = fireEvents;}

		void enableReading();
		void enableWriting();
		void disableWriting();
		void disableAll();
		bool isWriting() const;

		// for debug
		std::string reventsToString() const;
	private:
		EventLoop* loop_;
		const int fd_;
		int events_;
		int fireEvents_;
		ReadEventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback closeCallback_;
		EventCallback errorCallback_;
	private:
		DISALLOW_COPY_AND_ASSIGN(Session);
	};
}

}
#endif
