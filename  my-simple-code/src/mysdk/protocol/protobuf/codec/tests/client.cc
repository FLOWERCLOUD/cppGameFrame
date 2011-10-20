
#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/protocol/protobuf/codec/dispatcher.h>
#include <mysdk/protocol/protobuf/codec/codec.h>

#include <mysdk/protocol/protobuf/codec/tests/query.pb.h>


#include <mysdk/net/EventLoop.h>
#include <mysdk/net/SocketsOps.h>
#include <mysdk/net/TcpClient.h>

#include <stdio.h>

using namespace mysdk;
using namespace mysdk::net;

google::protobuf::Message* messageToSend;

class QueryClient
{
public:
	QueryClient(EventLoop* loop,
						const InetAddress& serverAddr):
			loop_(loop),
			client_(loop, serverAddr, "QueryClient"),
		    dispatcher_(std::tr1::bind(&QueryClient::onUnknownMessage,
		    		this,
		    		std::tr1::placeholders::_1,
		    		std::tr1::placeholders::_2,
		    		std::tr1::placeholders::_3)),

		    codec_(std::tr1::bind(&ProtobufDispatcher::onProtobufMessage,
		    		&dispatcher_,
		    		std::tr1::placeholders::_1,
		    		std::tr1::placeholders::_2,
		    		std::tr1::placeholders::_3))
	{
	    dispatcher_.registerMessageCallback<mysdk::Answer>(
	        std::tr1::bind(&QueryClient::onAnswer,
	        		this,
	        		std::tr1::placeholders::_1,
	        		std::tr1::placeholders::_2,
	        		std::tr1::placeholders::_3));

	    dispatcher_.registerMessageCallback<mysdk::Empty>(
	        std::tr1::bind(&QueryClient::onEmpty,
	        		this,
	        		std::tr1::placeholders::_1,
	        		std::tr1::placeholders::_2,
	        		std::tr1::placeholders::_3));

	    client_.setConnectionCallback(
	        std::tr1::bind(&QueryClient::onConnection,
	        		this,
	        		std::tr1::placeholders::_1));

	    client_.setMessageCallback(
	        std::tr1::bind(&ProtobufCodec::onMessage,
	        		&codec_,
	        		std::tr1::placeholders::_1,
	        		std::tr1::placeholders::_2,
	        		std::tr1::placeholders::_3));
	}

	void connect()
	{
		client_.connect();
	}

private:
	void onConnection(TcpConnection* pCon)
	{
		LOG_INFO << pCon->localAddress().toHostPort() << " -> "
				<< pCon->peerAddress().toHostPort() << " is "
				<< (pCon->connected() ? "UP" : "DOWN");

		if (pCon->connected())
		{
			codec_.send(pCon, *messageToSend);
		}
		else
		{
			loop_->quit();
		}
	}

	void onUnknownMessage(TcpConnection*,
														google::protobuf::Message* message,
														Timestamp)
	{
		LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
	}

	void onAnswer(mysdk::net::TcpConnection*,
									mysdk::Answer* message,
									mysdk::Timestamp)
	{
		LOG_INFO << "onAnswer:\n" << message->GetTypeName() << message->DebugString();
	}

	void onEmpty(mysdk::net::TcpConnection*,
								mysdk::Empty* message,
								mysdk::Timestamp)
	{
		LOG_INFO << "onEmpty: " << message->GetTypeName();
	}

private:
	EventLoop* loop_;
	TcpClient client_;
	ProtobufDispatcher dispatcher_;
	ProtobufCodec codec_;
private:
	DISALLOW_COPY_AND_ASSIGN(QueryClient);
};

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		EventLoop loop;
		uint16 port = static_cast<uint16>(atoi(argv[2]));
		InetAddress serverAddr(argv[1], port);

		mysdk::Query query;
	    query.set_id(1);
	    query.set_questioner("&&&&zjx&&&&");
	    query.add_question("Running?");
	    mysdk::Empty empty;
	    messageToSend = &query;

	    if (argc > 3 && argv[3][0] == 'e')
	    {
	      messageToSend = &empty;
	    }

	    QueryClient client(&loop, serverAddr);
	    client.connect();
	    loop.loop();
	}
	else
	{
		printf("Usage: %s host port [q|e]\n", argv[0]);
	}

	return 0;
}
