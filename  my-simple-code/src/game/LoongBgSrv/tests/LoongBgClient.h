/*
 * LoongBgClient.h
 *
 *  Created on: 2012-4-19
 *    
 */

#ifndef GAME_LOONGBGCLIENT_H_
#define GAME_LOONGBGCLIENT_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/TcpClient.h>
#include <mysdk/protocol/kabu/codec/KaBuCodec.h>

#include <string>

using namespace mysdk;
using namespace mysdk::net;

class LoongBgClient
{
public:
	LoongBgClient(EventLoop* loop, const InetAddress& listenAddr,
			int32 playerId, const std::string& name, int32 roleType, int32 times, int32 bgId, int32 team);
	~LoongBgClient(){}

	void connect();
	int32 getPlayerId();
	std::string getName();
	void setPlayerId(int32 playerId);
	void setName(const std::string& name);

private:
	void onEnterBattle(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onSelect(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onAddPlayer(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onRemovePlayer(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onReqBattleInfo(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onReqPlayerList(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onPing(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp);
	void onHurt(mysdk::net::TcpConnection* pCon, PacketBase& pb);
	void onMove(mysdk::net::TcpConnection* pCon, PacketBase& pb);

public:
	enum eState
	{
		NONE_STATE	= 0,
		START_STATE	= 1,
		SELECT_STATE	= 2, // 选择英雄
		MOVE_STATE		= 3, // 移动
		RESELECT_STATE = 4, // 重新选择英雄
		ATTACK_STATE	= 5, // 攻击
	};
private:
	void onConnection(mysdk::net::TcpConnection* pCon);
	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp);
	void onMsgHandler(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp);

	void onAIHandler();
	void onPingHandler();
	void onAttack();
	void onStandHandler();

	void useSkill();
private:
	void sendPacket(PacketBase& pb);
private:
	int32 playerId_;
	std::string name_;
	int32 roleType_;
	int32 times_;
	int32 bgId_; // 战场ID
	int32 team_; //队伍 （1 	为暗黑军 2 为烈阳军）
	int32 petId_; //英雄ID
	int32 hp_; //英雄血量
	int32 x_;
	int32 y_;
	std::vector<int> skillList_; //英雄技能列表
	eState state_;
	int32 attacker_; //攻击者
private:
	Timestamp sTimestamp_;
private:
	EventLoop* loop_;
	TcpClient client_;
	mysdk::net::TcpConnection* pCon_;
	KaBuCodec codec_;
private:
	DISALLOW_COPY_AND_ASSIGN(LoongBgClient);
};

#endif /* LOONGBGCLIENT_H_ */
