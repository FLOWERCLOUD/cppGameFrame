
#ifndef GAME_BATTLEGROUND_H_
#define GAME_BATTLEGROUND_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/scene/Scene.h>
#include <game/LoongBgSrv/BattlegroundState.h>
#include <game/LoongBgSrv/BgUnit.h>


class BgPlayer;
class Battleground
{
public:
	// 最大队伍人数
	static const uint8 sMaxTeamNum = 5;
	//
	static const uint8 sMinTeamNum  = 0;
public:
	Battleground();
	~Battleground();

	void setId(int32 Id);
	int32 getId();

	bool addBgPlayer(BgPlayer* player, BgUnit::TeamE team);
	bool removeBgPlayer(BgPlayer* player, BgUnit::TeamE team);

	void run(uint32 curTime);
	void shtudown();

	bool getBgInfo(PacketBase& op);

	BattlegroundState::BgStateE getState();
	uint32 getLeftTime();
//
public:
	// 战场释放结束啦
	bool isGameOver();
	// 战场结束结算
	void settlement();
	void incBgPlayerTimes();
	void tellClientBgState();
	bool isFull();
	bool isEmpty();

	bool haveOtherTeamEmpty();

    void switchWaitState();
    void switchCountDownState();
    void switchStartState();
    void switchRunState();
    void switchExitState();
    void closeBattleground();

private:
    void init();
    // 战场状态切换
    void setBattlegroundState(BattlegroundState* state);
private:
	int32 id_; // 战场ID
	uint8 teamNum_[BgUnit::kCOUNT_TEAM];
	bool bFirst_;
	BattlegroundState* pState_;
	Scene scene_; //战场所在场景 目前只支持一个战场地图哦
private:
	DISALLOW_COPY_AND_ASSIGN(Battleground);
};

#endif /* BATTLEGROUND_H_ */
