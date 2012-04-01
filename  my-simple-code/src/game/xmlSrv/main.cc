

#include <game/xmlSrv/XmlSrv.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#define XMLSRVVERSION	0.1
int main()
{
	LOG_INFO << "============ Start XmlSrv ============ version "  << XMLSRVVERSION;
	EventLoop loop;
	InetAddress listenAddr(843);
	XmlSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	LOG_INFO << "============  Stop XmlSrv ============ version "  << XMLSRVVERSION;
	return 0;
}
