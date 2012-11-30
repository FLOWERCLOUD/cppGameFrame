
#include <game/xmlSrv/XmlSrv.h>

#include <string.h>
#include <signal.h>

static bool shutdown_loongBgSrv = false;
static void sigtermHandler(int sig)
{
    LOG_WARN << "Received SIGTERM, scheduling shutdown...";
    shutdown_loongBgSrv = true;
}

void setupSignalHandlers(void)
{
	//LOG_WARN << "setupSignalHandlers ...";

    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
#ifdef HAVE_BACKTRACE
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
#endif
    return;
}

XmlSrv::XmlSrv(EventLoop* loop, InetAddress& serverAddr):
	loop_(loop),
	server_(loop, serverAddr, "XmlSrv")
{
	server_.setConnectionCallback(std::tr1::bind(&XmlSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&XmlSrv::onMessage,
														this,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));

    loop->runEvery(1, std::tr1::bind(&XmlSrv::tickMe, this));
}

XmlSrv::~XmlSrv()
{
}

void XmlSrv::start()
{
	setupSignalHandlers();
	server_.start();
}

void XmlSrv::stop()
{
}

void XmlSrv::onConnectionCallback(mysdk::net::TcpConnection* conn)
{
	LOG_DEBUG << "XmlSrv::onConnectionCallback " ;
	if (conn)
	{
		LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
				<< conn->localAddress().toHostPort() << " is"
				<< (conn->connected() ? "UP" : "DOWN");
	}
}

void XmlSrv::onMessage(mysdk::net::TcpConnection* pCon,
									mysdk::net::Buffer* buf,
									mysdk::Timestamp receiveTime)
{
	LOG_DEBUG << "XmlSrv::onMessage - bytes: " << buf->readableBytes();
    char data[200];
    buf->get(data, static_cast<int>(buf->readableBytes()));
    LOG_DEBUG << "XmlSrv::onMessage - data: " << data;
    static const char policy[] = "<policy-file-request/>";
    static const char cross[] ="<?xml version=\"1.0\"?> \
    		<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\"> \
    		<!-- Policy file for xmlsocket://socks.example.com --> \
			<cross-domain-policy> \
				<!-- This is a master socket policy file --> \
				<!-- No other socket policies on the host will be permitted --> \
				<!--site-control permitted-cross-domain-policies=\"all\"/--> \
				<!-- Instead of setting to-ports=\"*\", administrator's can use ranges and commas --> \
				<!-- This will allow access to ports 123, 456, 457 and 458 --> \
				<allow-access-from domain=\"*\" to-ports=\"*\" /> \
			</cross-domain-policy>";

    if (strcmp(data, policy) == 0)
    {
    	pCon->send(cross, strlen(cross));
    }
}

void XmlSrv::tickMe()
{
	if (shutdown_loongBgSrv)
	{
		loop_->quit();
	}
}
