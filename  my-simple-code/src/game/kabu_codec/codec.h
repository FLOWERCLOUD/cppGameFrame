
#ifndef CODEC_H
#define CODEC_H

#include <google/protobuf/message.h>
#include <string>
// struct KabuCodecTransportFormat __attribute__ ((__packed__))
// {
//   int32  len;
//   int16  nameLen;
//   char    typeName[nameLen];
//   int16  headlen;
//   char   headcontent[headlen];
//   char   protobufData[len-nameLen-headlen - 2*sizeof(int16)];
// }

class KabuCodec
{
public:
	enum ErrorCode
	{
		kNoError	=	0,
		kInvalidLength,
		kInvalidNameLen,
		kInvalidHeadLen,
		kUnknownMessageType,
		kParseError,
	};

	static const std::string& errorCodeToString(ErrorCode errorCode);
	static google::protobuf::Message* createMessage(const std::string& type_name);
	static google::protobuf::Message* createDynamicMessage(const std::string& typeName);
	static google::protobuf::Message* parse(const char* buf, int len, ErrorCode* errorCode);

	const static int kHeaderLen = sizeof(int32_t);
	const static int kMinMessageLen = kHeaderLen; //
	const static int kMaxMessageLen = 64 * 1024 * 1024;  // same as codec_stream.h kDefaultTotalBytesLimit
};

#endif
