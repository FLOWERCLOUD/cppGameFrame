
#include <mysdk/protocol/protobuf/codec/dispatcher.h>

#include <mysdk/protocol/protobuf/codec/tests/query.pb.h>

#include <iostream>

using std::cout;
using std::endl;

void onQuery(const mysdk::net::TcpConnection* pCon,
							const mysdk::Query* message,
							mysdk::Timestamp)
{
	cout << "onQuery: " << message->GetTypeName() << endl;
}

void onAnswer(const mysdk::net::TcpConnection* pCon,
								const mysdk::Answer* message,
								mysdk::Timestamp)
{
	cout << "onAnswer:" << message->GetTypeName() << endl;
}

void onUnknownMessageType(const mysdk::net::TcpConnection*,
															const google::protobuf::Message* message,
															mysdk::Timestamp)
{
	cout << "onUnknownMessageType: " << message->GetTypeName() << endl;
}

int main()
{
	  GOOGLE_PROTOBUF_VERIFY_VERSION;

	  ProtobufDispatcher dispatcher(onUnknownMessageType);
	  dispatcher.registerMessageCallback<mysdk::Query>(onQuery);
	  dispatcher.registerMessageCallback<mysdk::Answer>(onAnswer);

	  mysdk::net::TcpConnection* pCon = NULL;
	  mysdk::Timestamp t;

	  mysdk::Query* query = new mysdk::Query();
	  mysdk::Answer* answer = new mysdk::Answer();
	  mysdk::Empty* empty = new mysdk::Empty();
	  dispatcher.onProtobufMessage(pCon, query, t);
	  dispatcher.onProtobufMessage(pCon, answer, t);
	  dispatcher.onProtobufMessage(pCon, empty, t);

	  google::protobuf::ShutdownProtobufLibrary();
}

