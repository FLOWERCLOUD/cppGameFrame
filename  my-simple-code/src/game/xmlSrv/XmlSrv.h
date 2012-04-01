
#ifndef GAME_XMLSRV_H_
#define GAME_XMLSRV_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>
#include <mysdk/net/TcpConnection.h>

using namespace mysdk;
using namespace mysdk::net;


class XmlSrv
{
public:
	XmlSrv(EventLoop* loop, InetAddress& serverAddr);
	~XmlSrv();

public:
	void start();
	void stop();

	void onConnectionCallback(mysdk::net::TcpConnection* conn);
	void onMessage(mysdk::net::TcpConnection* pCon,
										mysdk::net::Buffer* buf,
										mysdk::Timestamp receiveTime);
private:
	void tickMe();

	EventLoop* loop_;
	TcpServer server_;
private:
	DISALLOW_COPY_AND_ASSIGN(XmlSrv);
};

#endif /* LOONGBGSRV_H_ */
