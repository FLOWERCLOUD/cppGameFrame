
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <google/protobuf/message.h>

#include <map>
#include <tr1/functional>

#include "LogThread.h"

class Callback
{
public:
	virtual ~Callback() {};
	virtual void onMessage(int conId,
													google::protobuf::Message* message) const = 0;
};

template <typename T>
class CallbackT: public Callback
{
public:
	typedef std::tr1::function<void (int conId,
																T* message)> ProtobufMessageTCallback;

	CallbackT(const ProtobufMessageTCallback& callback):
		callback_(callback)
	{
	}

	virtual void onMessage(int conId,
													google::protobuf::Message* message) const
	{
		LOG_TRACE << message->DebugString();

	    T* concrete = static_cast<T*>(message);
	    callback_(conId, concrete);
	}

private:
	ProtobufMessageTCallback callback_;
};

class ProtobufDispatcher
{
public:
	typedef std::tr1::function<void (int conId,
															google::protobuf::Message* message)> ProtobufMessageCallback;

	explicit ProtobufDispatcher(const ProtobufMessageCallback& defaultCb):
			defaultCallback_(defaultCb)
	{
	}

	~ProtobufDispatcher()
	{
		CallbackMap::iterator it = callbacks_.begin();
		for ( ; it != callbacks_.end(); ++it)
		{
			Callback* callback = it->second;
			if (callback)
			{
				delete callback;
			}
		}
	}

	void onProtobufMessage(int conId,
													google::protobuf::Message* message) const
	{
	    CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
	    if (it != callbacks_.end())
	    {
	    	it->second->onMessage(conId, message);
	    }
	    else
	    {
	    	defaultCallback_(conId, message);
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

