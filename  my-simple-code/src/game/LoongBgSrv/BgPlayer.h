/*
 * BgPlayer.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_BGPLAYER_H_
#define GAME_BGPLAYER_H_

#include <game/LoongBgSrv/BgUnit.h>
#include <game/LoongBgSrv/package/Package.h>
#include <game/LoongBgSrv/Battleground.h> // for BgResultE

#include <mysdk/net/TcpConnection.h>
#include <string>
#include <list>
#include <map>

class LoongBgSrv;
class Scene;
class BgPlayer:public BgUnit
{
public:
	BgPlayer(int32 playerId, std::string& playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	BgPlayer(int32 playerId, char* playerName, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	BgPlayer(int32 playerId, char* playerName, int32 roleType, int32 joinTimes, mysdk::net::TcpConnection* pCon, LoongBgSrv* pSrv);
	virtual ~BgPlayer();

	void setPetId(int16 petId);
	bool hasPet();

	void run(uint32 curTime);

	bool onMsgHandler(PacketBase& op);

	void setScene(Scene* scene);
	void setBgId(int16 bgId);
	void setRoleType(int32 roletype);
	void setJoinTimes(int32 joinTimes);

	void broadMsg(PacketBase& op);
	void close();
	void setWaitClose(bool flag);
	bool getWaitClose();

	bool addItem(int32 itemId);

	void shutdown();

	std::string getName()
	{
		return name_;
	}
	int16 getKillEnemyTimes()
	{
		return killEnemyTimes_;
	}

	void serializeResult(PacketBase& op, BgResultE bgResult, PacketBase& hotelop);
public:
	// 父类的东东
	virtual bool serialize(PacketBase& op);
	virtual bool canSkillHurt();
	virtual bool canBufHurt();
	virtual bool addBuf(Buf* buf);
	virtual bool hasSkill(int16 skillId);
	virtual bool canUseSkill(int16 skillId, int32 cooldownTime);
	virtual bool useSkill(int16 skillId);
	virtual void onHurt(BgUnit* attacker, int32 damage, const SkillBase& skill);
	virtual void onBufHurt(BgUnit* me, int32 damage, const BufBase& buf);
	virtual void incKillEnemyTime();
	virtual void fullHp();
	virtual void sendPacket(PacketBase& op);
private:
	void runBuf(uint32 curTime);
	void removeAllBuf();

	// 消息处理函数
	void onEnterBattle(PacketBase& pb);
	void onMove(PacketBase& pb);
	void onChat(PacketBase& pb);
	void onReqBattleInfo(PacketBase& pb);
	void onStand(PacketBase& pb);
	void onReqPlayerList(PacketBase& pb);
	void onSelectPet(PacketBase& pb);
	void onUseSkill(PacketBase& pb);
	void onExitBattle(PacketBase& pb);
	void onPickUpItem(PacketBase& pb);
	void onUseItem(PacketBase& pb);
private:
	std::string name_; //玩家的名字
	int16 killEnemyTimes_;
	int16 petId_;
	int16 battlegroundId_; //当然玩家所在的战场ID
	int32 roleType_;
	int16 title_;
	int32 joinTimes_; //该玩家已经玩了多少次了
	std::list<Buf*> bufList_; //玩家中的buf 列表
	std::map<int16, int32> useSkillMap_;
	bool bWaitClose_;
	Package package_;
	Scene* pScene;
	TcpConnection* pCon_;
	LoongBgSrv* pSrv_;
private:
	DISALLOW_COPY_AND_ASSIGN(BgPlayer);
};

#endif /* BGPLAYER_H_ */
