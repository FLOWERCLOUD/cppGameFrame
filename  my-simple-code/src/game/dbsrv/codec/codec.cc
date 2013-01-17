
#include <game/dbsrv/codec/codec.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/SocketsOps.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

using namespace mysdk;
using namespace mysdk::net;

// ByteSizeConsistencyError and InitializationErrorMessage are
// copied from google/protobuf/message_lite.cc

// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Authors: wink@google.com (Wink Saville),
//          kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.


// When serializing, we first compute the byte size, then serialize the message.
// If serialization produces a different number of bytes than expected, we
// call this function, which crashes.  The problem could be due to a bug in the
// protobuf implementation but is more likely caused by concurrent modification
// of the message.  This function attempts to distinguish between the two and
// provide a useful error message.
void ByteSizeConsistencyError(int byte_size_before_serialization,
                              int byte_size_after_serialization,
                              int bytes_produced_by_serialization)
{
  GOOGLE_CHECK_EQ(byte_size_before_serialization, byte_size_after_serialization)
      << "Protocol message was modified concurrently during serialization.";
  GOOGLE_CHECK_EQ(bytes_produced_by_serialization, byte_size_before_serialization)
      << "Byte size calculation and serialization were inconsistent.  This "
         "may indicate a bug in protocol buffers or it may be caused by "
         "concurrent modification of the message.";
  GOOGLE_LOG(FATAL) << "This shouldn't be called if all the sizes are equal.";
}

std::string InitializationErrorMessage(const char* action,
                                       const google::protobuf::MessageLite& message)
{
  // Note:  We want to avoid depending on strutil in the lite library, otherwise
  //   we'd use:
  //
  // return strings::Substitute(
  //   "Can't $0 message of type \"$1\" because it is missing required "
  //   "fields: $2",
  //   action, message.GetTypeName(),
  //   message.InitializationErrorString());

  std::string result;
  result += "Can't ";
  result += action;
  result += " message of type \"";
  result += message.GetTypeName();
  result += "\" because it is missing required fields: ";
  result += message.InitializationErrorString();
  return result;
}

int16 asInt16(const char* buf)
{
  int16_t be16 = 0;
  ::memcpy(&be16, buf, sizeof(be16));
  return be16;
  //return sockets::networkToHost16(be16);
}

int32 asInt32(const char* buf)
{
	  int32_t be32 = 0;
	  ::memcpy(&be32, buf, sizeof(be32));
	  return be32;
}

std::string decodeTypeName(const char* buf, int bufLen)
{
	if (bufLen >= 1024) return "";

	char tmp[1024];
	for (int i = 0; i < bufLen; i++)
	{
		tmp[i] = (char)(buf[i] ^ 0xFF);
	}
	tmp[bufLen] = 0;
	return tmp;
}

std::string ecodeTypeName(const char* buf, int bufLen)
{
	if (bufLen >= 1024) return "";

	char tmp[1024];
	for (int i = 0; i < bufLen; i++)
	{
		tmp[i] = (char)(buf[i] ^ 0xFF);
	}
	tmp[bufLen] = 0;
	return tmp;
}

void KabuCodec::fillEmptyBuff(Buffer* pBuf, google::protobuf::Message* message)
{
	assert(pBuf->readableBytes() == 0);

	const std::string& typeName = message->GetTypeName();

	int16 nameLen = static_cast<int16>(typeName.size());
	pBuf->appendInt16(nameLen);
	std::string eTypeName = ecodeTypeName(typeName.c_str(), nameLen);
	pBuf->append(eTypeName.c_str(), nameLen);

	// head
	pBuf->appendInt32(0);

	// code copied from MessageLite::SerializeToArray() and MessageLite::SerializePartialToArray().
	GOOGLE_DCHECK(message->IsInitialized()) << InitializationErrorMessage("serialize", (*message));

	int byte_size = message->ByteSize();
	pBuf->ensureWritableBytes(byte_size);

	  uint8* start = reinterpret_cast<uint8*>(pBuf->beginWrite());
	  uint8* end = message->SerializeWithCachedSizesToArray(start);
	  if (end - start != byte_size)
	  {
	    ByteSizeConsistencyError(byte_size, message->ByteSize(), static_cast<int>(end - start));
	  }
	  pBuf->hasWritten(byte_size);

	  int32 len = (static_cast<int32_t>(pBuf->readableBytes()));
	  pBuf->prepend(&len, sizeof len);
}

//
// no more google code after this
//

namespace
{
  const string kNoErrorStr = "NoError";
  const string kInvalidLengthStr = "InvalidLength";
  const string kCheckSumErrorStr = "CheckSumError";
  const string kInvalidNameLenStr = "InvalidNameLen";
  const string kUnknownMessageTypeStr = "UnknownMessageType";
  const string kParseErrorStr = "ParseError";
  const string kUnknownErrorStr = "UnknownError";
}

const string& KabuCodec::errorCodeToString(ErrorCode errorCode)
{
  switch (errorCode)
  {
   case kNoError:
     return kNoErrorStr;
   case kInvalidLength:
     return kInvalidLengthStr;
   case kInvalidNameLen:
     return kInvalidNameLenStr;
   case kUnknownMessageType:
     return kUnknownMessageTypeStr;
   case kParseError:
     return kParseErrorStr;
   default:
     return kUnknownErrorStr;
  }
}

void KabuCodec::defaultErrorCallback(TcpConnection* pCon,
                                         mysdk::net::Buffer* buf,
                                         mysdk::Timestamp,
                                         ErrorCode errorCode)
{
  LOG_ERROR << "ProtobufCodec::defaultErrorCallback - " << errorCodeToString(errorCode);
  if (pCon && pCon->connected())
  {
	  pCon->shutdown();
  }
}

void KabuCodec::onMessage(TcpConnection* pCon,
																Buffer* buf,
																Timestamp receiveTime)
{
	while (buf->readableBytes() >= kMinMessageLen + kHeaderLen)
	{
		const int32 len = buf->peekInt32();
		if (len > kMaxMessageLen || len < kMinMessageLen)
		{
			errorCallback_(pCon, buf, receiveTime, kInvalidLength);
			break;
		}
		else if (buf->readableBytes() >= implicit_cast<size_t>(len + kHeaderLen))
		{
			ErrorCode errorCode = kNoError;
			google::protobuf::Message* message = parse(buf->peek() + kHeaderLen, len, &errorCode);
			if (errorCode == kNoError && message)
			{
		        messageCallback_(pCon, message, receiveTime);
		        buf->retrieve(kHeaderLen+len);
			}
			else
			{
				errorCallback_(pCon, buf, receiveTime, errorCode);
				break;
			}
		}
		else
		{
			break;
		}
	}
}

google::protobuf::Message* KabuCodec::createMessage(const std::string& typeName)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor =
			google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
	if (descriptor)
	{
		const google::protobuf::Message* prototype =
				google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	// 从动态编译中找找看看吧
	if (!message)
	{
		message = createDynamicMessage(typeName);
	}
	return message;
}

void KabuCodec::init()
{
}

#include <game/dbsrv/ProtoImporter.h>
google::protobuf::Message* KabuCodec::createDynamicMessage(const std::string& typeName)
{
	  google::protobuf::Message* message = NULL;
	  const google::protobuf::Descriptor* descriptor = sProtoImporter.importer.pool()->FindMessageTypeByName(typeName);
	  if (descriptor)
	  {
	     const google::protobuf::Message* prototype = sProtoImporter.factory.GetPrototype(descriptor);
			if (prototype)
			{
				message = prototype->New();
			}
	  }
	  return message;
}

google::protobuf::Message* KabuCodec::parse(const char* buf, int len, ErrorCode* error)
{
	google::protobuf::Message* message = NULL;
	int16_t nameLen = asInt16(buf);
	int16_t bodyLeftLen = static_cast<int16_t>(len - sizeof(int16_t));
	if (nameLen >= 1 && nameLen <= bodyLeftLen)
	{
		std::string typeName = decodeTypeName(buf + sizeof(int16_t), nameLen);
		// create message object
		message = createMessage(typeName);
		//message = createDynamicMessage(typeName);
		if (message)
		{
		       // parse from buffer ( + sizeof(int32_t) 跳过头的长度 )
		       const char* data = buf + sizeof(int16_t) + nameLen + sizeof(int32_t) ;
		        int32 dataLen = static_cast<int32>(len - (sizeof(int16_t) + nameLen + sizeof(int32_t)));
		        ::google::protobuf::io::ArrayInputStream zeroCopy(static_cast<const void*>(data), dataLen);
		        if(message->ParseFromZeroCopyStream(&zeroCopy))
		        {
		          *error = kNoError;
		        }
		        else
		        {
		          *error = kParseError;
		        }
		}
		else
		{
			*error = kUnknownMessageType;
		}
	}
	else
	{
		*error = kInvalidNameLen;
	}

	return message;
}
