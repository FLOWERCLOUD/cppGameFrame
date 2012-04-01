
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/util/md5.h>
#include <game/LoongBgSrv/BattlegroundMgr.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/PlayerMgr.h>
#include <game/LoongBgSrv/Util.h>

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

LoongBgSrv::LoongBgSrv(EventLoop* loop, InetAddress& serverAddr):
	loop_(loop),
	server_(loop, serverAddr, "LoongBgSrv"),
	codec_(
				std::tr1::bind(&LoongBgSrv::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	server_.setConnectionCallback(std::tr1::bind(&LoongBgSrv::onConnectionCallback,
																this,
																std::tr1::placeholders::_1));

	server_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
														&codec_,
														std::tr1::placeholders::_1,
														std::tr1::placeholders::_2,
														std::tr1::placeholders::_3));

    loop->runEvery(0.5, std::tr1::bind(&LoongBgSrv::tickMe, this));

	LOG_DEBUG << "============ LoongBgSrv::LoongBgSrv ================";
}

LoongBgSrv::~LoongBgSrv()
{
	LOG_DEBUG << "============ LoongBgSrv::~LoongBgSrv ================";
}

void LoongBgSrv::start()
{
	setupSignalHandlers();
	server_.start();
}

void LoongBgSrv::stop()
{
	sBattlegroundMgr.shutdown();
}

void LoongBgSrv::onConnectionCallback(mysdk::net::TcpConnection* conn)
{
	if (conn)
	{
		LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
				<< conn->localAddress().toHostPort() << " is"
				<< (conn->connected() ? "UP" : "DOWN");
		// 玩家断开了连接
		if (!conn->connected())
		{
			BgPlayer* player = static_cast<BgPlayer*>(conn->getContext());
			if (player != NULL)
			{
				player->close();
				//
				delete player;
			}
			conn->setContext(NULL);
		}
	}
}

void LoongBgSrv::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 op = pb.getOP();
	if (op == game::OP_LOGIN)
	{
		login(pCon, pb, timestamp);
	}
	else
	{
		BgPlayer* player = static_cast<BgPlayer*>(pCon->getContext());
		if (player != NULL)
		{
			TIME_FUNCTION_CALL(player->onMsgHandler(pb), 10);
		}
	}
}

bool LoongBgSrv::login(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	int32 playerId = pb.getInt32();
	char playerName[128];
	if (!pb.getUTF(playerName, 127))
	{
		return false;
	}

	int32 roleType = pb.getInt32();
	int32 times = pb.getInt32();

	char token[100];
	if (!pb.getUTF(token, 99))
	{
		return false;
	}

	LOG_DEBUG << "LoongBgSrv::login - playerId:"  << playerId
							<< " playerName: " << playerName
							<< " roleType: " << roleType
							<< " times: " << times
							<< " address: "<< pCon->peerAddress().toHostPort();

	static const char key[] =  "9B1492CF6AAE903F63FB7759D3565CD7";
	char buf[256];
	//md5(uid + username + roletype + times +  key);
	snprintf(buf, 255, "%d%s%d%d%s", playerId, playerName, roleType, times, key);
	char* tmp = MD5String(buf);
	if (strcmp(tmp, token) != 0)
	{
			LOG_ERROR << "LoongBgSrv::login - playerId:"  << playerId
									<< " playerName: " << playerName
									<< " token: " << token
									<< " times: " << times
									<< " servmd5: " << tmp
									<< " buf: " << buf
									<< " address: "<< pCon->peerAddress().toHostPort();
			free(tmp);
			return false;
	}
	free(tmp);

	BgPlayer* player = new BgPlayer(playerId, playerName, pCon, this);
	if (!player) return false;

	player->setRoleType(roleType);
	player->setTimes(static_cast<int16>(times));
	pCon->setContext(player);
	//  告诉客户端 登陆成功哦
	PacketBase op(client::OP_LOGIN, 0);
	op.putInt32(0); // 登陆成功
	send(pCon, op);
	return true;
}

void LoongBgSrv::tickMe()
{
	if (!shutdown_loongBgSrv)
	{
		uint32 curTime = getCurTime();
		TIME_FUNCTION_CALL(sBattlegroundMgr.run(curTime), 10);
	}
	else
	{
		loop_->quit();
	}
}

void LoongBgSrv::send(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	codec_.send(pCon, pb);
}
