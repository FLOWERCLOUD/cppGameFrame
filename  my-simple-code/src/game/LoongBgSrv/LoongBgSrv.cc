
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <game/LoongBgSrv/base/Base.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/util/md5.h>
#include <game/LoongBgSrv/BattlegroundMgr.h>
#include <game/LoongBgSrv/BgPlayer.h>
#include <game/LoongBgSrv/JoinTimesMgr.h>
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

#include <game/LoongBgSrv/php/htmlclient.h>
CHtmlClient htmlClient;

void LoongBgSrv::phpThreadHandler()
{
	LOG_INFO << "====== phpThreadHandler STARTING ";
	while (!shutdown_loongBgSrv)
	{
		ThreadParam param = queue_.take();
		int32 id = param.bgId;
		//htmlClient.loadUrl("http://www.baidu.com");
		//LOG_TRACE << " Test html: " << htmlClient.getHtmlData().getData();
		LOG_TRACE << "phpThreadHandler - id: " << id;
	}
	LOG_INFO << "======= phpThreadHandler END ";
}

LoongBgSrv::LoongBgSrv(EventLoop* loop, InetAddress& serverAddr):
	codec_(
				std::tr1::bind(&LoongBgSrv::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3)),
	phpThread_(std::tr1::bind(&LoongBgSrv::phpThreadHandler, this), "phpThread"),
	battlegroundMgr_(this),
	loop_(loop),
	server_(loop, serverAddr, "LoongBgSrv")
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
    loop->runEvery(8.0, std::tr1::bind(&LoongBgSrv::onTimer, this));
	LOG_DEBUG << "============ LoongBgSrv::LoongBgSrv ================";
}

LoongBgSrv::~LoongBgSrv()
{
	LOG_DEBUG << "============ LoongBgSrv::~LoongBgSrv ================";
}

void LoongBgSrv::start()
{
	std::string host = "192.168.100.6";
	std::string port_or_socket = "3306";
	std::string user = "root";
	std::string password = "4399mysql#CQPZM";
	std::string database = "kabu_loongBg";

	sBase.init(host, port_or_socket, user, password, database);
	setupSignalHandlers();
	phpThread_.start();
	server_.start();
}

void LoongBgSrv::stop()
{
	ThreadParam x;
	x.bgId = 0;
	queue_.put(x);
	phpThread_.join();
}

void LoongBgSrv::onConnectionCallback(mysdk::net::TcpConnection* pCon)
{
	if (pCon)
	{
		LOG_TRACE << pCon->peerAddress().toHostPort() << " -> "
				<< pCon->localAddress().toHostPort() << " is"
				<< (pCon->connected() ? "UP" : "DOWN");
		// 玩家断开了连接
		if (!pCon->connected())
		{
			BgClient* bgClient = static_cast<BgClient*>(pCon->getContext());
			if (bgClient)
			{
				bgClientList_.erase(bgClient->iter);
				BgPlayer* player = bgClient->player;
				if (player != NULL)
				{
					int32 playerId = player->getId();
					if (!player->getWaitClose())
					{
						bgPlayerMap_.erase(playerId);
					}

					player->close();
					//
					delete player;
				}
			}
			delete bgClient;
			pCon->setContext(NULL);
		}
		else
		{
			BgClient* bgClient = new BgClient();
			if (bgClient)
			{
				bgClientList_.push_back(bgClient);

				bgClient->iter = --bgClientList_.end();
				bgClient->player = NULL;
				bgClient->lastRecvTimestamp = Timestamp::now();
				bgClient->pCon = pCon;

				pCon->setContext(bgClient);
			}
			else
			{
				pCon->close();
			}
		}
	}
}

void LoongBgSrv::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 op = pb.getOP();
	if (op == game::OP_MY_TICKET)
	{
		BgClient* bgClient = static_cast<BgClient*>(pCon->getContext());
		if (bgClient)
		{
			std::list<BgClient* >::iterator iter = bgClient->iter;
			bgClientList_.erase(iter);
			bgClientList_.push_back(bgClient);
			bgClient->iter = --bgClientList_.end();
			bgClient->lastRecvTimestamp = timestamp;
		}
		return;
	}

	if (op == game::OP_LOGIN)
	{
		bool flag;
		TIME_FUNCTION_CALL( flag = login(pCon, pb, timestamp), 10);
		if (!flag)
		{
			pCon->close();
		}
	}
	else
	{
		BgClient* bgClient = static_cast<BgClient*>(pCon->getContext());
		if (bgClient != NULL)
		{
			BgPlayer* player = bgClient->player;
			if (player)
			{
				TIME_FUNCTION_CALL(player->onMsgHandler(pb), 10);
			}
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
							<< " token: " << token
							<< " address: "<< pCon->peerAddress().toHostPort();

	static const char key[] =  "9B1492CF6AAE903F63FB7759D3565CD7";
	char buf[256];
	//md5(uid + username + roletype + times +  key);
	snprintf(buf, 255, "%d%s%d%d%s", playerId, playerName, roleType, times, key);
	char* tmp = MD5String(buf);
	if (strcmp(tmp, token) != 0)
	{
			LOG_ERROR << "LoongBgSrv::login(md5 error) - playerId:"  << playerId
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

	// 参加次数限制( 如果以后做成多进程，参战次数要保存在一个公共的地方中去)
	static const int32 maxJoinTimes = 5;
	int32 joinTimes = sJoinTimesMgr.getJoinTimes(playerId);
	if ( joinTimes > maxJoinTimes)
	{
		LOG_ERROR << "LoongBgSrv::login (too much join Times) - playerId:"  << playerId
								<< " playerName: " << playerName
								<< " joinTimes: " << joinTimes
								<< " address: "<< pCon->peerAddress().toHostPort();

		PacketBase op(client::OP_LOGIN, 0);
		op.putInt32(-2); // 已经玩5次啦 不能再玩啦
		send(pCon, op);

		return false;
	}

	joinTimes = joinTimes > times ? joinTimes : times;
	LOG_TRACE << "LoongBgSrv::login - playerId:"  << playerId
							<< " playerName: " << playerName
							<< " roleType: " << roleType
							<< " jointimes: " << joinTimes
							<< " token: " << token
							<< " address: "<< pCon->peerAddress().toHostPort();

	// 这个玩家是否已经在战场上了
	bool bInBg = false;
	if (hasBgPlayer(playerId))
	{
		LOG_DEBUG << "LoongBgSrv::login(in battle) - playerId:"  << playerId
								<< " playerName: " << playerName
								<< " roleType: " << roleType
								<< " jointimes: " << joinTimes
								<< " address: "<< pCon->peerAddress().toHostPort();
		bInBg = true;
		BgPlayer* bgPlayer = bgPlayerMap_[playerId];
		if (bgPlayer)
		{
			bgPlayer->setWaitClose(true);

			PacketBase op(client::OP_LOGIN, 0);
			op.putInt32(-3); //异地登陆啦
			bgPlayer->sendPacket(op);

			bgPlayer->shutdown();
		}
	}

	BgPlayer* player = new BgPlayer(playerId, playerName, roleType, joinTimes, pCon, this);
	if (!player) return false;
	if (bInBg)
	{
		bgPlayerMap_[playerId] = player;
		LOG_DEBUG << "LoongBgSrv::login(in battle) - === playerId:"  << playerId
					<< " player: " << player;
	}
	else
	{
		bgPlayerMap_.insert(std::pair<int32, BgPlayer*>(playerId, player));
	}

	BgClient* bgClient = static_cast<BgClient*>(pCon->getContext());
	assert(bgClient);
	bgClient->player = player;

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
		TIME_FUNCTION_CALL(battlegroundMgr_.run(curTime), 10);
	}
	else
	{
		loop_->quit();
	}
}

void LoongBgSrv::onTimer()
{
	Timestamp now = Timestamp::now();
	std::list<BgClient* >::iterator itList;
	for (itList = bgClientList_.begin(); itList != bgClientList_.end(); ++itList)
	{
		BgClient* bgClient = *itList;
		if (timeDifference(now, bgClient->lastRecvTimestamp) > 8.0)
		{
			mysdk::net::TcpConnection* pCon = bgClient->pCon;
			if (pCon)
			{
				LOG_TRACE << pCon->peerAddress().toHostPort() << " -> "
						<< pCon->localAddress().toHostPort() << " recover!!";

				pCon->close();
			}
		}
		else
		{
			break;
		}
	}
}

void LoongBgSrv::send(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	codec_.send(pCon, pb);
}

bool LoongBgSrv::hasBgPlayer(int32 playerId)
{
	BgPlayerMapT::iterator iter;
	iter = bgPlayerMap_.find(playerId);
	if (iter != bgPlayerMap_.end())
	{
		return true;
	}
	return false;
}
