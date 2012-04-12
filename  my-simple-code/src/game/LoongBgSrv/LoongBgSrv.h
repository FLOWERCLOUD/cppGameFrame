
#ifndef GAME_LOONGBGSRV_H_
#define GAME_LOONGBGSRV_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/BlockingQueue.h>
#include <mysdk/base/PerformanceCounter.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Thread.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>
#include <mysdk/net/TcpConnection.h>

#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

#include <game/LoongBgSrv/BattlegroundMgr.h>
#include <game/LoongBgSrv/Hotel.h>

#include <list>
#include <map>

using namespace mysdk;
using namespace mysdk::net;

struct ThreadParam
{
	int16 bgId;
	uint8 blackNum;
	uint8 whiteNum;
	int16 bgState;
};

class BgPlayer;

struct BgClient
{
	Timestamp lastRecvTimestamp;
	std::list<BgClient* >::iterator iter;
	BgPlayer* player;
	mysdk::net::TcpConnection* pCon;

	Timestamp lastSecPacketsTimestamp; //收到包的上一秒的时间
	uint32 lastSecondsPackets; //上一秒收到的包数
};

// 龙族对抗战场服务器
class LoongBgSrv
{
public:
	typedef std::map<int32, BgPlayer*> BgPlayerMapT;
public:
	LoongBgSrv(EventLoop* loop, InetAddress& serverAddr);
	~LoongBgSrv();

public:
	void start();
	void stop();

	void onConnectionCallback(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);
	void onWriteComplete(mysdk::net::TcpConnection* pCon);
	void printThroughput();

	void send(mysdk::net::TcpConnection* pCon, PacketBase& pb);

	BattlegroundMgr& getBgMgr()
	{
		return battlegroundMgr_;
	}

	void TellPhpBattleInfo(int32 bgId);
	void sendToHotel(PacketBase& pb);
private:
	bool login(mysdk::net::TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);
	void tickMe();
	void onTimer();
	bool hasBgPlayer(int32 playerId);
	void phpThreadHandler();
private:
	KaBuCodec codec_;
	BgPlayerMapT bgPlayerMap_;
	Thread phpThread_;
	BattlegroundMgr battlegroundMgr_;
	std::list<BgClient*> bgClientList_;

	BlockingQueue<ThreadParam> queue_;

	// @start 统计相关
	int64 tmpTransferred_;
	int64 transferred_;
	Timestamp startTime_;
	PerformanceCounter performanceCounter_;
	// @end 统计相关
	Hotel hotel_;

	EventLoop* loop_;
	TcpServer server_;
private:
	DISALLOW_COPY_AND_ASSIGN(LoongBgSrv);
};

#endif /* LOONGBGSRV_H_ */
