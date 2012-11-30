
#ifndef PROTOCOL_PROTOBUF_CODEC_CODEC_H
#define PROTOCOL_PROTOBUF_CODEC_CODEC_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Buffer.h>
#include <mysdk/net/TcpConnection.h>

#include <google/protobuf/message.h>

#include <string>
#include <tr1/functional>

// struct ProtobufTransportFormat __attribute__ ((__packed__))
// {
//   int32  len;
//   int16  nameLen;
//   char    typeName[nameLen];
//   int16  headlen;
//   char   headcontent[headlen];
//   char   protobufData[len-nameLen-headlen];
// }

class KabuCodec
{
public:
	enum ErrorCode
	{
		kNoError	=	0,
		kInvalidLength,
		kCheckSumError,
		kInvalidNameLen,
		kUnknownMessageType,
		kParseError,
	};

	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
			google::protobuf::Message*,
			mysdk::Timestamp)>  KabuMessageCallback;

	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
			mysdk::net::Buffer*,
			mysdk::Timestamp,
			ErrorCode)> ErrorCallback;

	explicit KabuCodec(const  KabuMessageCallback& messageCb):
			messageCallback_(messageCb),
			errorCallback_(defaultErrorCallback)
	{
	}

	KabuCodec(const  KabuMessageCallback& messageCb,
			const ErrorCallback& errorCb):
			messageCallback_(messageCb),
			errorCallback_(errorCb)
	{
	}

	void onMessage(mysdk::net::TcpConnection* pCon,
									mysdk::net::Buffer* pBuf,
									mysdk::Timestamp receiveTime);

	void send(mysdk::net::TcpConnection* pCon,
						google::protobuf::Message* message)
	{
		 // FIXME: serialize to TcpConnection::outputBuffer()
		mysdk::net::Buffer buf;
		fillEmptyBuff(&buf, message);
		pCon->send(&buf);
	}

	static const std::string& errorCodeToString(ErrorCode errorCode);
	static void fillEmptyBuff(mysdk::net::Buffer* pBuf, google::protobuf::Message* message);
	static google::protobuf::Message* createMessage(const std::string& type_name);

	static google::protobuf::Message* parse(const char* buf, int len, ErrorCode* errorCode);

private:
	static void defaultErrorCallback(mysdk::net::TcpConnection* pCon,
																mysdk::net::Buffer*,
																mysdk::Timestamp,
																ErrorCode);

	KabuMessageCallback messageCallback_;
	ErrorCallback errorCallback_;

	const static int kHeaderLen = sizeof(int32_t);
	const static int kMinMessageLen = kHeaderLen; //
	const static int kMaxMessageLen = 64 * 1024 * 1024;  // same as codec_stream.h kDefaultTotalBytesLimit

private:
	DISALLOW_COPY_AND_ASSIGN(KabuCodec);
};

#endif
