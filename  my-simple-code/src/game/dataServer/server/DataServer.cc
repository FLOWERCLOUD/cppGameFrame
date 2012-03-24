/*
 * DataServer.cc
 *
 *  Created on: 2011-10-31
 *    
 */

#include <game/dataServer/server/DataServer.h>

#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/TcpConnection.h>

#include <tr1/functional>
#include <string>

using namespace DataServer;

DataServer::DataServer(EventLoop* loop, InetAddress& addr):
		loop_(loop),
		server_(loop, addr, "DataServer"),
		dispatcher_(
								std::tr1::bind(&DataServer::onUnknownMessage,
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
    dispatcher_.registerMessageCallback<dataserver::Select>(
    	std::tr1::bind(&DataServer::onSelect,
    			this,
    			std::tr1::placeholders::_1,
    			std::tr1::placeholders::_2,
    			std::tr1::placeholders::_3));

    dispatcher_.registerMessageCallback<dataserver::Insert>(
    	std::tr1::bind(&DataServer::onInsert,
    			this,
    			std::tr1::placeholders::_1,
    			std::tr1::placeholders::_2,
    			std::tr1::placeholders::_3));

    dispatcher_.registerMessageCallback<dataserver::Delete>(
    	std::tr1::bind(&DataServer::onDelete,
    			this,
    			std::tr1::placeholders::_1,
    			std::tr1::placeholders::_2,
    			std::tr1::placeholders::_3));

    dispatcher_.registerMessageCallback<dataserver::Update>(
    	std::tr1::bind(&DataServer::onUpdate,
    			this,
    			std::tr1::placeholders::_1,
    			std::tr1::placeholders::_2,
    			std::tr1::placeholders::_3));

	server_.setConnectionCallback(std::tr1::bind(&DataServer::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&DataServer::onMessageCallback,
														this,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));
}

DataServer::~DataServer()
{
}

void DataServer::start()
{
	server_.start();
}

void DataServer::onConnectionCallback(TcpConnection* conn)
{
	LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
			<< conn->localAddress().toHostPort() << " is"
			<< (conn->connected() ? "UP" : "DOWN");
}

void DataServer::onUnknownMessage(TcpConnection* conn, google::protobuf::Message* message, Timestamp timestamp)
{
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
    conn->shutdown();
}

void DataServer::onSelect(TcpConnection* conn, dataserver::Select* selectMsg, Timestamp timestamp)
{
	int32 key = slectMsg->key();
	std::string msgName(slectMsg->messagename());
}

void DataServer::onInsert(TcpConnection* conn, dataserver::Insert* insertMsg, Timestamp timestamp)
{
	int32 key = insertMsg->key();
	std::string msgName(insertMsg->messagename());
}

void DataServer::onDelete(TcpConnection* conn, dataserver::Delete* deleteMsg, Timestamp timestamp)
{
	int32 key = deleteMsg->key();
	std::string msgName(deleteMsg->messagename());
}

void DataServer::onUpdate(TcpConnection* conn, dataserver::Update* updateMsg, Timestamp timestamp)
{
	int32 key = updateMsg->key();
	std::string msgName(updateMsg->messagename());
}


////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	if (argc > 1)
	{
		EventLoop loop;
		uint16 port = static_cast<uint16>(atoi(argv[1]));
		InetAddress serverAddr(port);
		DataServer server(&loop, serverAddr);
		server.start();
		loop.loop();
	}
	else
	{
		printf("Usage: %s port\n", argv[0]);
	}

	google::protobuf::ShutdownProtobufLibrary();
}
