
#ifndef MYSDK_PROTOCOL_KABU_CODEC_CODEC_H
#define MYSDK_PROTOCOL_KABU_CODEC_CODEC_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Buffer.h>
#include <mysdk/net/TcpConnection.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <string>
#include <tr1/functional>

using namespace std;
using namespace mysdk;
// struct KaBuTransportFormat __attribute__ ((__packed__))
// {
//   int16  magic;
//   int16  len;
//   uint32 op;
//   uint32 param;
// }

class KaBuCodec
{
public:
	enum ErrorCode
	{
		kNoError	=	0,
		kInvalidLength,
		kParseError,
		kUnSupportError,
	};

	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
			PacketBase& ,
			mysdk::Timestamp)> KaBuMessageCallback;

	typedef std::tr1::function<void (mysdk::net::TcpConnection*,
			mysdk::net::Buffer*,
			mysdk::Timestamp,
			ErrorCode)> ErrorCallback;

	explicit KaBuCodec(const KaBuMessageCallback& messageCb):
			messageCallback_(messageCb),
			errorCallback_(defaultErrorCallback)
	{
	}

	KaBuCodec(const KaBuMessageCallback& messageCb,
			const ErrorCallback& errorCb):
			messageCallback_(messageCb),
			errorCallback_(errorCb)
	{
	}

	void onMessage(mysdk::net::TcpConnection* pCon,
									mysdk::net::Buffer* pBuf,
									mysdk::Timestamp receiveTime);

	void send(mysdk::net::TcpConnection* pCon, PacketBase& pb);

	static const std::string& errorCodeToString(ErrorCode errorCode);
private:
	static void defaultErrorCallback(mysdk::net::TcpConnection* pCon,
																mysdk::net::Buffer*,
																mysdk::Timestamp,
																ErrorCode);

	KaBuMessageCallback messageCallback_;
	ErrorCallback errorCallback_;

	const static uint16 kHeaderLen = 2 * sizeof(int16)+ 2 * sizeof(int32);
	const static uint16 kMinMessageLen = 0;
	const static uint16 kMaxMessageLen = 63 * 1024;
private:
	DISALLOW_COPY_AND_ASSIGN(KaBuCodec);
};

#endif
