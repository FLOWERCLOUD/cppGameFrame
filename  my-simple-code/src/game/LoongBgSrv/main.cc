

#include <game/LoongBgSrv/LoongBgSrv.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#define 	LOONGBGSRVVERSION	0.1
int main()
{
	LOG_INFO << "============ Start LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	EventLoop loop;
	InetAddress listenAddr(2007);
	LoongBgSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	LOG_INFO << "============  Stop LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	return 0;
}
