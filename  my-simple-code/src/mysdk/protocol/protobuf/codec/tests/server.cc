
#include <mysdk/protocol/protobuf/codec/codec.h>
#include <mysdk/protocol/protobuf/codec/dispatcher.h>

#include <mysdk/protocol/protobuf/codec/tests/query.pb.h>

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/SocketsOps.h>
#include <mysdk/net/TcpServer.h>

#include <stdio.h>

using namespace mysdk;
using namespace mysdk::net;

class QueryServer
{
public:
	QueryServer(EventLoop* loop,
							const InetAddress& listenAddr):
			loop_(loop),
			server_(loop, listenAddr, "QueryServer"),
			dispatcher_(
					std::tr1::bind(&QueryServer::onUnknownMessage,
											this,
											std::tr1::placeholders::_1,
											std::tr1::placeholders::_2,
											std::tr1::placeholders::_3)),

			codec_(
					std::tr1::bind(&ProtobufDispatcher::onProtobufMessage,
											&dispatcher_,
											std::tr1::placeholders::_1,
											std::tr1::placeholders::_2,
											std::tr1::placeholders::_3))
	{
	    dispatcher_.registerMessageCallback<mysdk::Query>(
	    	std::tr1::bind(&QueryServer::onQuery,
	    			this,
	    			std::tr1::placeholders::_1,
	    			std::tr1::placeholders::_2,
	    			std::tr1::placeholders::_3));

	    dispatcher_.registerMessageCallback<mysdk::Answer>(
	    	std::tr1::bind(&QueryServer::onAnswer,
	    			this,
	    			std::tr1::placeholders::_1,
	    			std::tr1::placeholders::_2,
	    			std::tr1::placeholders::_3));

	    server_.setConnectionCallback(
	        std::tr1::bind(&QueryServer::onConnection,
	        		this,
	        		std::tr1::placeholders::_1));

	    server_.setMessageCallback(
	        std::tr1::bind(&ProtobufCodec::onMessage,
	        		&codec_,
	        		std::tr1::placeholders::_1,
	        		std::tr1::placeholders::_2,
	        		std::tr1::placeholders::_3));
	}

	void start()
	{
		server_.start();
	}
private:
	void onConnection(TcpConnection* pCon)
	{
	    LOG_INFO << pCon->localAddress().toHostPort() << " -> "
	        << pCon->peerAddress().toHostPort() << " is "
	        << (pCon->connected() ? "UP" : "DOWN");
	}

	void onUnknownMessage(TcpConnection* pCon,
														google::protobuf::Message* message,
														Timestamp)
	{
	    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
	    pCon->shutdown();

	    loop_->quit();
	}

	void onQuery(mysdk::net::TcpConnection* pCon,
								mysdk::Query* message,
								mysdk::Timestamp)
	{
	    LOG_INFO << "onQuery:\n" << message->GetTypeName() << message->DebugString();
	    Answer answer;
	    answer.set_id(1);
	    answer.set_questioner("&&&&zjx&&&&");
	    answer.set_answerer("my name unknow!!!");
	    answer.add_solution("Jump!");
	    answer.add_solution("Win!");
	    codec_.send(pCon, answer);

	    pCon->shutdown();
	}

	void onAnswer(mysdk::net::TcpConnection* pCon,
									mysdk::Answer* message,
									mysdk::Timestamp)
	{
	    LOG_INFO << "onAnswer: " << message->GetTypeName();
	    pCon->shutdown();
	}

	EventLoop* loop_;
	TcpServer server_;
	ProtobufDispatcher dispatcher_;
	ProtobufCodec codec_;
private:
	DISALLOW_COPY_AND_ASSIGN(QueryServer);
};

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	if (argc > 1)
	{
		EventLoop loop;
		uint16 port = static_cast<uint16>(atoi(argv[1]));
		InetAddress serverAddr(port);
		QueryServer server(&loop, serverAddr);
		server.start();
		loop.loop();
	}
	else
	{
		printf("Usage: %s port\n", argv[0]);
	}

	google::protobuf::ShutdownProtobufLibrary();
}
