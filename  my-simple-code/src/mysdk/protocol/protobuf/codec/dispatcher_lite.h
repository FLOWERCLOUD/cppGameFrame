
#ifndef MYSDK_PROTOCOL_PROTOBUF_CODEC_DISPATCHER_LITE_H
#define MYSDK_PROTOCOL_PROTOBUF_CODEC_DISPATCHER_LITE_H

#include <mysdk/net/Callbacks.h>

#include <mysdk/base/Common.h>

#include <google/protobuf/message.h>

#include <map>
#include <tr1/functional>

class ProtobufDispatcherLite
{
public:
	typedef std::tr1::function<void (const mysdk::net::TcpConnection*,
														const google::protobuf::Message*,
														mysdk::Timestamp)> ProtobufMessageCallback;

	explicit ProtobufDispatcherLite(const ProtobufMessageCallback& defaultCb):
			defaultCallback_(defaultCb)
	{
	}

	void onProtobufMessage(const mysdk::net::TcpConnection* pCon,
														const google::protobuf::Message* message,
														mysdk::Timestamp receiveTime) const
	{
	    CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
	    if (it != callbacks_.end())
	    {
	      it->second(pCon, message, receiveTime);
	    }
	    else
	    {
	      defaultCallback_(pCon, message, receiveTime);
	    }
	}

	void registerMessageCallback(const google::protobuf::Descriptor* desc,
																const ProtobufMessageCallback& callback)
	{
		callbacks_[desc] = callback;
	}

private:
	typedef std::map<const google::protobuf::Descriptor*, ProtobufMessageCallback> CallbackMap;
	CallbackMap callbacks_;
	ProtobufMessageCallback defaultCallback_;
private:
	DISALLOW_COPY_AND_ASSIGN(ProtobufDispatcherLite);
};

#endif

