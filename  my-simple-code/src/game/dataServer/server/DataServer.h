/*
 * DataServer.h
 *
 *  Created on: 2011-10-31
 *    
 */

#ifndef DATASERVER_H_
#define DATASERVER_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>

#include <mysdk/protocol/protobuf/codec/codec.h>
#include <mysdk/protocol/protobuf/codec/dispatcher.h>

#include <game/dataServer/server/dataServer.pb.h>

namespace DataServer
{

using namespace mysdk;
using namespace mysdk::net;

class DataServer
{
public:
	DataServer(EventLoop* loop, InetAddress& serverAddr);
	~DataServer();

public:
	void start();

private:
	void onConnectionCallback(TcpConnection*);
	void onUnknownMessage(TcpConnection* conn, google::protobuf::Message* message, Timestamp timestamp);

	void onSelect(TcpConnection* conn, dataserver::Select* message, Timestamp timestamp);
	void onInsert(TcpConnection* conn, dataserver::Insert* message, Timestamp timestamp);
	void onDelete(TcpConnection* conn, dataserver::Delete* message, Timestamp timestamp);
	void onUpdate(TcpConnection* conn, dataserver::Update* message, Timestamp timestamp);
private:
	EventLoop* loop_;
	TcpServer server_;
	ProtobufDispatcher dispatcher_;
	ProtobufCodec codec_;
private:
	DISALLOW_COPY_AND_ASSIGN(DataServer);
};

}

#endif /* DATASERVER_H_ */
