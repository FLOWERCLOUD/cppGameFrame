
#include <game/dbsrv/DBSrv.h>

#include <game/dbsrv/WorkerThread.h>
#include <game/dbsrv/version.h>

#include <string.h>
#include <signal.h>
//#define HAVE_BACKTRACE

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif

#ifdef HAVE_BACKTRACE
#pragma GCC diagnostic ignored "-Wold-style-cast"
static void *getMcontextEip(ucontext_t *uc) {
#if defined(__FreeBSD__)
    return (void*) uc->uc_mcontext.mc_eip;
#elif defined(__dietlibc__)
    return (void*) uc->uc_mcontext.eip;
#elif defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
  #if __x86_64__
    return (void*) uc->uc_mcontext->__ss.__rip;
  #elif __i386__
    return (void*) uc->uc_mcontext->__ss.__eip;
  #else
    return (void*) uc->uc_mcontext->__ss.__srr0;
  #endif
#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
  #if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
    return (void*) uc->uc_mcontext->__ss.__rip;
  #else
    return (void*) uc->uc_mcontext->__ss.__eip;
  #endif
#elif defined(__i386__)
    return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
    return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
    return (void*) uc->uc_mcontext.sc_ip;
#else
    return NULL;
#endif
}

void bugReportStart()
{
	LOG_WARN << "=== DBSrv BUG REPORT START: Cut & paste starting from here ===";
}

void bugReportEnd()
{
	  LOG_WARN <<  "=== DBSrv BUG REPORT END. Make sure to include from START to END. ===";
}

static void sigsegvHandler(int sig, siginfo_t *info, void *secret)
{
    ucontext_t *uc = (ucontext_t*) secret;
    struct sigaction act;

    bugReportStart();
    LOG_WARN << "    DBSrv "  << DBSRVVERSION << " crashed by signal: " << sig;

	void *trace[100];
    /* Generate the stack trace */
    int trace_size = backtrace(trace, 100);

    /* overwrite sigaction with caller's address */
    if (getMcontextEip(uc) != NULL) {
        trace[1] = getMcontextEip(uc);
    }
    char **messages = backtrace_symbols(trace, trace_size);
    LOG_WARN << "--- STACK TRACE" ;
    for (int i=1; i<trace_size; ++i)
    {
    	 LOG_WARN << messages[i];
    }

    bugReportEnd();

    /* Make sure we exit with the right signal at the end. So for instance
     * the core will be dumped if enabled. */
    sigemptyset (&act.sa_mask);
    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
     * is used. Otherwise, sa_handler is used */
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;
    sigaction (sig, &act, NULL);
    kill(getpid(),sig);
}
#pragma GCC diagnostic error "-Wold-style-cast"
#endif /* HAVE_BACKTRACE */

static bool shutdownSrv = false;
static void sigtermHandler(int sig)
{
    LOG_WARN << "Received SIGTERM, scheduling shutdown...";
    shutdownSrv = true;
}

void setupSignalHandlers(void)
{
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

DBSrv::DBSrv(EventLoop* loop, InetAddress& serverAddr, int workthreadnum, int writethreadnum):
	codec_(
			std::tr1::bind(&DBSrv::onKaBuMessage,
			this,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2,
			std::tr1::placeholders::_3)),
	loop_(loop),
	server_(loop, serverAddr, "DBSrv"),
	threadPool_(this, workthreadnum),
	writeThreadPool_(writethreadnum)
{
	server_.setConnectionCallback(std::tr1::bind(&DBSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&KabuCodec::onMessage,
														&codec_,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));

	 loop->runEvery(1, std::tr1::bind(&DBSrv::tickMe, this));
	 loop->runEvery(10, std::tr1::bind(&DBSrv::ping, this));

	 mysql_library_init(0, NULL, NULL);
}

DBSrv::~DBSrv()
{
	mysql_library_end();
}

void DBSrv::tickMe()
{
	if (shutdownSrv)
	{
		loop_->quit();
	}
}

void DBSrv::ping()
{
	threadPool_.ping();
	writeThreadPool_.ping();
}

void DBSrv::start()
{
	setupSignalHandlers();
	threadPool_.start();
	writeThreadPool_.start();
	server_.start();
}

void DBSrv::stop()
{
	threadPool_.stop();
	writeThreadPool_.stop();
}

static int nextid = 0;
void DBSrv::onConnectionCallback(mysdk::net::TcpConnection* pCon)
{
	if (!pCon) return;
	if (pCon->connected())
	{
		int conid = nextid++;
		if (conid >= 90000000) conid = 0;

		Context* context = new Context();
		if (!context) return;

		context->conId = conid;
		pCon->setContext(context);
		conMap_.insert(std::pair<int, mysdk::net::TcpConnection*>(conid, pCon));
	}
	else
	{
		Context* context = static_cast<Context*>(pCon->getContext());
		assert(context);
		conMap_.erase(context->conId);
		delete context;
	}
}


void DBSrv::onKaBuMessage(mysdk::net::TcpConnection* pCon,
		google::protobuf::Message* msg,
		mysdk::Timestamp timestamp)
{
	if (!pCon) return;

	Context* context = static_cast<Context*>(pCon->getContext());
	if (!context) return;

	struct ThreadParam param;
	param.Type = CMD;
	param.msg = msg;
	param.conId = context->conId;
	threadPool_.push(param);
}

void DBSrv::sendReply(int conId, google::protobuf::Message* message)
{
	assert(message);

	ConMapT::iterator iter;
	iter = conMap_.find(conId);
	if (iter == conMap_.end())
	{
		// 这个连接断开过？？
	    // 现在可以释放掉这个消息了
	    delete message;
		return;
	}

	mysdk::net::TcpConnection* pCon = iter->second;
    codec_.send(pCon, message);
    // 现在可以释放掉这个消息了
    delete message;
}

void DBSrv::sendReplyEx(int conId, mysdk::net::Buffer* pBuf)
{
	assert(pBuf);

	ConMapT::iterator iter;
	iter = conMap_.find(conId);
	if (iter == conMap_.end())
	{
		// 这个连接断开过？？
	    delete pBuf;
		return;
	}

	mysdk::net::TcpConnection* pCon = iter->second;
	if (!pCon)
	{
		// 这个连接竟然是null 不会吧
		delete pBuf;
		return;
	}

	pCon->send(pBuf);
    delete pBuf;
}
