
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <game/LoongBgSrv/config/ConfigMgr.h>
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
	static const std::string phpAddr(sConfigMgr.MainConfig.GetStringDefault("phpAddr", "path", "115.238.55.103/battlefield/info_in.php"));
	char buf[1024];
	while (!shutdown_loongBgSrv)
	{
		ThreadParam param = queue_.take();
		if (param.bgId != -1)
		{
			snprintf(buf, 1023, "%s?id=%d&type=3&black=%d&white=%d&states=%d",
				phpAddr.c_str(),
				param.bgId,
				param.whiteNum,
				param.blackNum,
				param.bgState);

			htmlClient.loadUrl(buf, 1);
			//LOG_TRACE << " Test html: " << htmlClient.getHtmlData().getData();
		}

		LOG_TRACE << "phpThreadHandler - id: " << param.bgId;
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
	tmpTransferred_(0),
    transferred_(0),
    startTime_(Timestamp::now()),
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

	server_.setWriteCompleteCallback(
			std::tr1::bind(&LoongBgSrv::onWriteComplete, this, std::tr1::placeholders::_1));

    loop->runEvery(0.5, std::tr1::bind(&LoongBgSrv::tickMe, this));
    loop->runEvery(8.0, std::tr1::bind(&LoongBgSrv::onTimer, this));
    loop->runEvery(3.0, std::tr1::bind(&LoongBgSrv::printThroughput, this));
	LOG_DEBUG << "============ LoongBgSrv::LoongBgSrv serverAddr: "<<  server_.hostport()
							<< " ================";
}

LoongBgSrv::~LoongBgSrv()
{
	LOG_DEBUG << "============ LoongBgSrv::~LoongBgSrv serverAddr: " <<  server_.hostport()
							<< " ================";
}

void LoongBgSrv::start()
{
	setupSignalHandlers();
	phpThread_.start();
	server_.start();
}

void LoongBgSrv::stop()
{
	ThreadParam x;
	x.bgId = -1;
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
				bgClient->lastSecPacketsTimestamp = Timestamp::now();
				bgClient->lastSecondsPackets = 0;

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
	BgClient* bgClient = static_cast<BgClient*>(pCon->getContext());
	if (!bgClient)
	{
		LOG_WARN << " LoongBgSrv::onKaBuMessage - SOME ERROR!!! "
								<< " address: "<< pCon->peerAddress().toHostPort();
		return;
	}

	std::list<BgClient* >::iterator iter = bgClient->iter;
	bgClientList_.erase(iter);
	bgClientList_.push_back(bgClient);
	bgClient->iter = --bgClientList_.end();
	bgClient->lastRecvTimestamp = timestamp;

	if (timeDifference(timestamp, bgClient->lastSecPacketsTimestamp) < 1.0)
	{
		bgClient->lastSecondsPackets++;
	}
	else
	{
		bgClient->lastSecPacketsTimestamp = timestamp;
		bgClient->lastSecondsPackets = 1;
	}

	static uint32 minLimitPacketPerClient = sConfigMgr.MainConfig.GetIntDefault("packet", "minLimit", 10);
	static uint32 maxLimitPacketPerClient = sConfigMgr.MainConfig.GetIntDefault("packet", "maxLimit", 20);
	if (bgClient->lastSecondsPackets > minLimitPacketPerClient)
	{
		LOG_WARN << " LoongBgSrv::onKaBuMessage - packet too much, so overlook "
								<< " lastSecondsPackets: " << bgClient->lastSecondsPackets
								<< " address: "<< pCon->peerAddress().toHostPort();

		// 玩家作弊了！？
		if (bgClient->lastSecondsPackets > maxLimitPacketPerClient)
		{
			LOG_WARN << " LoongBgSrv::onKaBuMessage - packet too much, so  close "
									<< " lastSecondsPackets: " << bgClient->lastSecondsPackets
									<< " address: "<< pCon->peerAddress().toHostPort();
			pCon->close();
		}
		return;
	}

	uint32 op = pb.getOP();
	if (op == game::OP_PING)
	{
		pb.setOP(client::OP_PING);
		send(pCon, pb);
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
		BgPlayer* player = bgClient->player;
		if (player)
		{
			TIME_FUNCTION_CALL(player->onMsgHandler(pb), 10);
		}
	}
}

void LoongBgSrv::onWriteComplete(mysdk::net::TcpConnection* pCon)
{
	transferred_ += tmpTransferred_;
	tmpTransferred_ = 0;
}

void LoongBgSrv::printThroughput()
{
	Timestamp endTime = Timestamp::now();
	double time = timeDifference(endTime, startTime_);
	printf("%4.3f KiB/s  -- transferred: %" MYSDK_LL_FORMAT "d B -- ConnectionNum: %d -- Cpu: %f%%-- RAM: %f M\n",
			static_cast<double>(transferred_)/time/1024,
			transferred_,
			server_.getConnectionNum(),
			performanceCounter_.GetCurrentCPUUsage(),
			performanceCounter_.GetCurrentRAMUsage());

	transferred_ = 0;
	startTime_ = endTime;
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

	//static const char key[] =  "9B1492CF6AAE903F63FB7759D3565CD7";
	static const std::string key(sConfigMgr.MainConfig.GetStringDefault("loginKey", "key", "9B1492CF6AAE903F63FB7759D3565CD7"));

	char buf[256];
	//md5(uid + username + roletype + times +  key);
	snprintf(buf, 255, "%d%s%d%d%s", playerId, playerName, roleType, times, key.c_str());
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

void LoongBgSrv::TellPhpBattleInfo(int32 battleId)
{
	int16 bgId = static_cast<int16>(battleId);
	if (battlegroundMgr_.checkBattlegroundId(bgId))
	{
		Battleground& bg = battlegroundMgr_.getBattleground(bgId);
		ThreadParam param;
		param.bgId = bgId;
		param.blackNum = bg.getBlackNum();
		param.whiteNum = bg.getWhiteNum();
		param.bgState = bg.getState();
		queue_.put(param);
	}
}

void LoongBgSrv::send(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	tmpTransferred_ += pb.getContentLen() + sizeof(PacketBase::KaBuHead);
	LOG_TRACE << "LoongBgSrv::send - tmpTransferred:" << tmpTransferred_;
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
