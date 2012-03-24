

#include <game/LoongBgSrv/LoongBgSrv.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

int main()
{
	EventLoop loop;
	InetAddress listenAddr(2007);
	LoongBgSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	return 0;
}
