
#ifndef GAME_BOSSSRV_H_
#define GAME_BOSSSRV_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/BlockingQueue.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Thread.h>
#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpServer.h>
#include <mysdk/net/TcpConnection.h>

#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

#include <game/bosssrv/BossHurtMgr.h>

using namespace mysdk;

struct ThreadParam
{
	uint32 cmd;
	char* param;
};

// 全民boss服务器
class BossSrv
{
public:
	BossSrv(EventLoop* loop, InetAddress& serverAddr);
	~BossSrv();

public:
	void start();
	void stop();

	void onConnectionCallback(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, Timestamp timestamp);

	void send(mysdk::net::TcpConnection* pCon, PacketBase& pb);

private:
	void tellPhpBossElem();
	void tellPhpPlayerHurt(uint32 uid, uint32 hurtvalue, char* username, uint32 flag);
	void tellPhpActOver();
	void tellPhpTop();
private:
	
        void updateHp();
	void tickMe();
	void broadMsg(PacketBase& pb);
	void phpThreadHandler();
	void jiesuan();
private:
	bool open_; //是否开启了活动
	bool haveAward_; //是否领取了奖励
	bool bUpdateHp_; //是否跟新boss血量
	uint32 bossId_;
	int32 bossHp_;
	uint32 bossElem_;
	int32 initBossHp_;

	BossHurtMgr bossHurtMgr_;
	BlockingQueue<ThreadParam> queue_;
	Thread phpThread_;

	KaBuCodec codec_;
	EventLoop* loop_;
	TcpServer server_;
private:
	DISALLOW_COPY_AND_ASSIGN(BossSrv);
};

#endif /* LOONGBGSRV_H_ */
