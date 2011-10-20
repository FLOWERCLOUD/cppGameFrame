
#include <mysdk/protocol/protobuf/codec/codec.h>
#include <mysdk/protocol/protobuf/codec/tests/query.pb.h>

#include <stdio.h>
#include <zlib.h>  // adler32

using namespace mysdk;
using namespace mysdk::net;

void print(const Buffer& buf)
{
  printf("encoded to %zd bytes\n", const_cast<Buffer&>(buf).readableBytes());
  for (size_t i = 0; i < const_cast<Buffer&>(buf).readableBytes(); ++i)
  {
    unsigned char ch = static_cast<unsigned char>(buf.peek()[i]);

    printf("%2zd:  0x%02x  %c\n", i, ch, isgraph(ch) ? ch : ' ');
  }
}

void testQuery()
{
  mysdk::Query query;
  query.set_id(1);
  query.set_questioner("Chen Shuo");
  query.add_question("Running?");

  Buffer buf;
  ProtobufCodec::fillEmptyBuff(&buf, query);
  print(buf);

  const int32_t len = buf.readInt32();
  assert(len == static_cast<int32_t>(buf.readableBytes()));

  ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
  google::protobuf::Message* message = ProtobufCodec::parse(buf.peek(), len, &errorCode);
  assert(errorCode == ProtobufCodec::kNoError);
  assert(message != NULL);
  message->PrintDebugString();
  assert(message->DebugString() == query.DebugString());

}

void testAnswer()
{
  mysdk::Answer answer;
  answer.set_id(1);
  answer.set_questioner("Chen Shuo");
  answer.set_answerer("blog.csdn.net/Solstice");
  answer.add_solution("Jump!");
  answer.add_solution("Win!");

  Buffer buf;
  ProtobufCodec::fillEmptyBuff(&buf, answer);
  print(buf);

  const int32_t len = buf.readInt32();
  assert(len == static_cast<int32_t>(buf.readableBytes()));

  ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
  google::protobuf::Message*  message = ProtobufCodec::parse(buf.peek(), len, &errorCode);
  assert(errorCode == ProtobufCodec::kNoError);
  assert(message != NULL);
  message->PrintDebugString();
  assert(message->DebugString() == answer.DebugString());
}

void testEmpty()
{
  mysdk::Empty empty;

  Buffer buf;
  ProtobufCodec::fillEmptyBuff(&buf, empty);
  print(buf);

  const int32_t len = buf.readInt32();
  assert(len == static_cast<int32_t>(buf.readableBytes()));

  ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
  google::protobuf::Message*  message = ProtobufCodec::parse(buf.peek(), len, &errorCode);
  assert(message != NULL);
  message->PrintDebugString();
  assert(message->DebugString() == empty.DebugString());
}

void redoCheckSum(string& data, int len)
{
  int32_t checkSum = ::htonl(static_cast<int32_t>(
      ::adler32(0,
                reinterpret_cast<const Bytef*>(data.c_str()),
                static_cast<int>(len - 4))));
  data[len-4] = reinterpret_cast<const char*>(&checkSum)[0];
  data[len-3] = reinterpret_cast<const char*>(&checkSum)[1];
  data[len-2] = reinterpret_cast<const char*>(&checkSum)[2];
  data[len-1] = reinterpret_cast<const char*>(&checkSum)[3];
}

void testBadBuffer()
{
  mysdk::Empty empty;
  empty.set_id(43);

  Buffer buf;
  ProtobufCodec::fillEmptyBuff(&buf, empty);
  // print(buf);

  const int32_t len = buf.readInt32();
  assert(len == static_cast<int32_t>(buf.readableBytes()));

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len-1, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kCheckSumError);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[len-1]++;
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kCheckSumError);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[0]++;
    google::protobuf::Message*message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kCheckSumError);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[3] = 0;
    redoCheckSum(data, len);
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kInvalidNameLen);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[3] = 100;
    redoCheckSum(data, len);
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kInvalidNameLen);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[3]--;
    redoCheckSum(data, len);
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kUnknownMessageType);
  }

  {
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    data[4] = 'M';
    redoCheckSum(data, len);
    google::protobuf::Message*message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kUnknownMessageType);
  }

  {
    // FIXME: reproduce parse error
    string data(buf.peek(), len);
    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::kNoError;
    redoCheckSum(data, len);
    google::protobuf::Message* message = ProtobufCodec::parse(data.c_str(), len, &errorCode);
    assert(message == NULL);
    assert(errorCode == ProtobufCodec::kParseError);
  }
}

int g_count = 0;

void onMessage(const mysdk::net::TcpConnection* pCon,
               const google::protobuf::Message* message,
               mysdk::Timestamp receiveTime)
{
  g_count++;
}

void testOnMessage()
{
  mysdk::Query query;
  query.set_id(1);
  query.set_questioner("Chen Shuo");
  query.add_question("Running?");

  Buffer buf1;
  ProtobufCodec::fillEmptyBuff(&buf1, query);

  mysdk::Empty empty;
  empty.set_id(43);
  empty.set_id(1982);

  Buffer buf2;
  ProtobufCodec::fillEmptyBuff(&buf2, empty);

  size_t totalLen = buf1.readableBytes() + buf2.readableBytes();
  Buffer all;
  all.append(buf1.peek(), buf1.readableBytes());
  all.append(buf2.peek(), buf2.readableBytes());
  assert(all.readableBytes() == totalLen);
  mysdk::net::TcpConnection* pCon = NULL;
  mysdk::Timestamp t;
  ProtobufCodec codec(onMessage);
  for (size_t len = 0; len <= totalLen; ++len)
  {
    Buffer input;
    input.append(all.peek(), len);

    g_count = 0;
    codec.onMessage(pCon, &input, t);
    int expected = len < buf1.readableBytes() ? 0 : 1;
    if (len == totalLen) expected = 2;
    assert(g_count == expected);
    // printf("%2zd %d\n", len, g_count);

    input.append(all.peek() + len, totalLen - len);
    codec.onMessage(pCon, &input, t);
    assert(g_count == 2);
  }
}

int main()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  testQuery();
  puts("");
  testAnswer();
  puts("");
  testEmpty();
  puts("");
  testBadBuffer();
  puts("");
  testOnMessage();
  puts("");

  puts("All pass!!!");

  google::protobuf::ShutdownProtobufLibrary();
}

