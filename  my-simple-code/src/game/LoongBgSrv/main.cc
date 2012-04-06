

#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/php/htmlclient.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#define 	LOONGBGSRVVERSION	0.1

int main()
{
	LOG_INFO << "============ Start LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	CHtmlClient::initialize();
	EventLoop loop;
	InetAddress listenAddr(2007);
	LoongBgSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	CHtmlClient::finalise();
	LOG_INFO << "============  Stop LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	return 0;
}
