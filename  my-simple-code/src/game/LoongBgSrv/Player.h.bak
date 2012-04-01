/*
 * Player.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PLAYER_H_
#define GAME_PLAYER_H_

#include <mysdk/base/Common.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <string>

class Buf;
class mysdk::net::TcpConnection;
class LoongBgSrv;
class Player
{
public:
	typedef enum tagTeamE
	{
		kNONE,
		kBlack,  //暗黑军
		kWhite,// 烈阳军
	} TeamE;
public:
	Player(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	Player(int32 playerId, std::string& playerName, TeamE team, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	~Player();

	int32 getPlayerId();
	Pet* getPet();

	std::string getPlayerName();

	int16 getX();
	int16 getY();
	int16 getKillEnemyTimes();
	void incKillEnemyTimes();
	// 伤害
	void onHurt(Player* attacker, int16 damage, int16 skillId);
	// 回血
	void onRiseHp(int16 hp);
	// 减血
	void onDecHp(int16 hp);

	void sendPacket(PacketBase& op);

	void removeBuffer(Buf* buf);
	void addBuffer(Buf* buf);
public:
	void update();
private:
	int32 playerId_; // 玩家ID
	std::string playerName_; //玩家的名字
	int16 x_; //玩家的x坐标点
	int16 y_; //玩家的y坐标点
	int16 killEnemyTimes_; //杀敌次数
	TeamE team_; //玩家所在队伍
	Pet* pet_; //玩家做的坐骑
	std::vector<Buf*> bufList_; //玩家中的buf 列表
	mysdk::net::TcpConnection* pCon_;
	LoongBgSrv* pSrv_;
private:
	DISALLOW_COPY_AND_ASSIGN(Player);
};

#endif /* PLAYER_H_ */
