
#ifndef MYSDK_PROTOCOL_PROTOBUF_CODEC_DISPATCHER_H
#define MYSKD_PROTOCOL_PROTOBUF_CODEC_DISPATCHER_H

#include <mysdk/net/Callbacks.h>

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <google/protobuf/message.h>

#include <map>
#include <tr1/functional>

using namespace mysdk;

class Callback
{
public:
	virtual ~Callback() {};
	virtual void onMessage(mysdk::net::TcpConnection*,
													google::protobuf::Message* message,
													mysdk::Timestamp) const = 0;
//private:
//	DISALLOW_COPY_AND_ASSIGN(Callback);
};

template <typename T>
class CallbackT: public Callback
{
public:
	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
																T* message,
																mysdk::Timestamp)> ProtobufMessageTCallback;

	CallbackT(const ProtobufMessageTCallback& callback):
		callback_(callback)
	{
	}

	virtual void onMessage(mysdk::net::TcpConnection* pCon,
													google::protobuf::Message* message,
													mysdk::Timestamp receiveTime) const
	{
	    T* concrete = static_cast<T*>(message);
	    assert(concrete != NULL);
	    callback_(pCon, concrete, receiveTime);
	}

private:
	ProtobufMessageTCallback callback_;
};

class ProtobufDispatcher
{
public:
	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
															google::protobuf::Message* message,
															mysdk::Timestamp)> ProtobufMessageCallback;

	explicit ProtobufDispatcher(const ProtobufMessageCallback& defaultCb):
			defaultCallback_(defaultCb)
	{
	}

	~ProtobufDispatcher()
	{
		LOG_TRACE << "ProtobufDispatcher::~ProtobufDispatcher";
		CallbackMap::iterator it = callbacks_.begin();
		for ( ; it != callbacks_.end(); ++it)
		{
			LOG_TRACE << "ProtobufDispatcher::~ProtobufDispatcher: " << it->first;
			Callback* callback = it->second;
			if (callback)
			{
				delete callback;
			}
		}
	}

	void onProtobufMessage(mysdk::net::TcpConnection* pCon,
													google::protobuf::Message* message,
													mysdk::Timestamp receiveTime) const
	{
		LOG_TRACE << "ProtobufDispatcher::onProtobufMessage";
	    CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
	    if (it != callbacks_.end())
	    {
	    	it->second->onMessage(pCon, message, receiveTime);
	    }
	    else
	    {
	    	defaultCallback_(pCon, message, receiveTime);
	    }
	}

	template <typename T>
	void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageTCallback& callback)
	{
	    callbacks_[T::descriptor()] = new CallbackT<T>(callback);
	}

private:
	typedef std::map<const google::protobuf::Descriptor*, Callback*> CallbackMap;
	CallbackMap callbacks_;
	ProtobufMessageCallback defaultCallback_;
};

#endif

