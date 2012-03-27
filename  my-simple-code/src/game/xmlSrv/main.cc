

#include <game/xmlSrv/XmlSrv.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

int main()
{
	EventLoop loop;
	InetAddress listenAddr(843);
	XmlSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	return 0;
}
