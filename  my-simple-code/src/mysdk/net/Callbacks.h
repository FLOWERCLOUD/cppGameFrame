
#ifndef MYSDK_NET_CALLBACKS_H
#define MYSDK_NET_CALLBACKS_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Logging.h>

#include <tr1/functional>

namespace mysdk
{
namespace net
{
	class Buffer;
	class TcpConnection;
	typedef std::tr1::function<void()> TimerCallback;
	typedef std::tr1::function<void (TcpConnection*)> ConnectionCallback;
	typedef std::tr1::function<void (TcpConnection*)> CloseCallback;
	typedef std::tr1::function<void (TcpConnection*)> WriteCompleteCallback;

	typedef std::tr1::function<void (TcpConnection*, Buffer*, Timestamp)> MessageCallback;

	void defaultConnectionCallback(TcpConnection*);
	void defaultMessageCallback(TcpConnection*, Buffer* buf, Timestamp timestamp);

}
}

#endif
