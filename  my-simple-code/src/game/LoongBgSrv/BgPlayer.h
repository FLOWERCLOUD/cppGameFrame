/*
 * BgPlayer.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_BGPLAYER_H_
#define GAME_BGPLAYER_H_

#include <game/LoongBgSrv/BgUnit.h>
#include <mysdk/net/TcpConnection.h>
#include <string>
#include <list>


class LoongBgSrv;
class Scene;
class BgPlayer:public BgUnit
{
public:
	BgPlayer(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	BgPlayer(int32 playerId, char* playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	virtual ~BgPlayer();

	void incKillEnemyTime();

	void setPetId(int16 petId);
	bool hasPet();

	void sendPacket(PacketBase& op);

	void run(uint32 curTime);

	bool onMsgHandler(PacketBase& op);

	void setScene(Scene* scene);
public:
	// 父类的东东
	virtual bool serialize(PacketBase& op);
	virtual bool canSkillHurt();
	virtual bool canBufHurt();
	virtual bool addBuf(Buf* buf);
	virtual bool hasSkill(int16 skillId);
	virtual bool canUseSkill(int16 skillId);
	virtual void onHurt(BgUnit* attacker, int32 damage, int16 skillId);

private:
	void runBuf(uint32 curTime);
	// 消息处理函数
	void onEnterBattle(PacketBase& pb);
	void onMove(PacketBase& pb);
	void onChat(PacketBase& pb);
private:
	std::string name_; //玩家的名字
	int16 killEnemyTimes_;
	int16 petId_;

	std::list<Buf*> bufList_; //玩家中的buf 列表

	Scene* pScene;
	TcpConnection* pCon_;
	LoongBgSrv* pSrv_;
private:
	DISALLOW_COPY_AND_ASSIGN(BgPlayer);
};

#endif /* BGPLAYER_H_ */
