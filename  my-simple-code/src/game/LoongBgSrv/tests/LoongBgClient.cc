/*
 * LoongBgClient.cc
 *
 *  Created on: 2012-4-19
 *    
 */

#include <game/LoongBgSrv/tests/LoongBgClient.h>

#include <mysdk/base/Logging.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/util/md5.h>
#include <game/LoongBgSrv/tests/BgClientDataMgr.h>
#include <game/LoongBgSrv/tests/BgClientData.h>
#include <game/LoongBgSrv/tests/StaticData.h>
#include <game/LoongBgSrv/Util.h>

#include <string.h>

LoongBgClient::LoongBgClient(EventLoop* loop, const InetAddress& listenAddr,
		int32 playerId, const std::string& name, int32 roleType, int32 times, int32 bgId, int32 team):
	playerId_(playerId),
	name_(name),
	roleType_(roleType),
	times_(times),
	bgId_(bgId),
	team_(team),
	state_(NONE_STATE),
	attacker_(0),
	loop_(loop),
	client_(loop, listenAddr, "LoongBgClient"),
	pCon_(NULL),
	codec_(
				std::tr1::bind(&LoongBgClient::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	client_.setConnectionCallback(
			std::tr1::bind(&LoongBgClient::onConnection, this, std::tr1::placeholders::_1));

	client_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
			&codec_,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2,
			std::tr1::placeholders::_3));

	loop->runEvery(1.0, std::tr1::bind(&LoongBgClient::onAIHandler, this));
	loop->runEvery(1.0, std::tr1::bind(&LoongBgClient::onPingHandler, this));
	loop->runEvery(1.0, std::tr1::bind(&LoongBgClient::onStandHandler, this));
}

void LoongBgClient::connect()
{
	client_.connect();
}

int32 LoongBgClient::getPlayerId()
{
	return playerId_;
}

std::string LoongBgClient::getName()
{
	return name_;
}

void LoongBgClient::setPlayerId(int32 playerId)
{
	playerId_ = playerId;
}

void LoongBgClient::setName(const std::string& name)
{
	name_  = name;
}

void LoongBgClient::onConnection(mysdk::net::TcpConnection* pCon)
{
	if (!pCon) return;
	LOG_INFO << pCon->localAddress().toHostPort() << " -> "
			<< pCon->peerAddress().toHostPort() << " is "
			<< (pCon->connected() ? "UP" : "DOWN");

	if (pCon->connected())
	{
		pCon_ = pCon;
		static char key[] = "9B1492CF6AAE903F63FB7759D3565CD7";
		char tmp[128];
		snprintf(tmp, 127, "%d%s%d%d%s", playerId_, name_.c_str(), roleType_, times_, key);

		char* token = MD5String(tmp);
		PacketBase op(game::OP_LOGIN, 0);
		op.putInt32(playerId_);
		op.putUTF(name_);
		op.putInt32(roleType_);
		op.putInt32(times_);
		op.putUTF(token, static_cast<int16>(strlen(token)));
		codec_.send(pCon, op);
		free(token);
	}
	else
	{
		pCon_ = NULL;
	}
}

void LoongBgClient::onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 op = pb.getOP();
	if (op == client::OP_LOGIN)
	{
		int32 result = pb.getInt32();
		if (result == 0)
		{
			LOG_INFO << name_ << " 登陆成功 ";
			LOG_INFO << name_ << "  进入战场 " << bgId_ << " team: " << team_ ;
			PacketBase pb1(game::OP_ENTER_BATTLE, 0);
			pb1.putInt32(bgId_);
			pb1.putInt32(team_);
			codec_.send(pCon, pb1);
		}
		else if (result == -1)
		{
			LOG_INFO << name_<< " 登陆失败 ";
			pCon->shutdown();
		}
		else if (result == -2)
		{
			LOG_INFO << name_ << " 已经玩5次啦 不能再玩啦 " ;
			pCon->shutdown();
		}
		else if (result == -3)
		{
			LOG_INFO << name_ << " 异地登陆";
			pCon->shutdown();
		}
	}
	else
	{
		onMsgHandler(pCon, pb, timestamp);
	}
}

void LoongBgClient::onAIHandler()
{
	if (state_ == SELECT_STATE)
	{
		randSelectHero();
	}
	else if (state_ == MOVE_STATE)
	{
		int32 head = getRandomBetween(0, 3);
		if (head == 0)
		{
			if (x_ < maxPos[0] - 2)
			{
				x_ += heroIdSpeed[petId_];
			}
		}
		else if (head == 1)
		{
			if (y_ < maxPos[1] - 2 )
			{
				y_ += heroIdSpeed[petId_];
			}
		}
		else if (head == 2)
		{
			if (x_ < 2)
			{
				x_  -= heroIdSpeed[petId_];
			}
		}
		else
		{
			if (y_ <  2 )
			{
				y_ -= heroIdSpeed[petId_];
			}
		}

		//// 在一定范围的敌人 攻击他一下吧
		onAttack();
	}
	else if (state_ == RESELECT_STATE)
	{
		randSelectHero();
	}
	else if (state_ == ATTACK_STATE)
	{
		useSkill();
	}
}

void LoongBgClient::randSelectHero()
{
	int32 heroNum = sizeof(heroIdList) / sizeof(int32) - 1;
	int32 petId = getRandomBetween(0, heroNum);
	PacketBase op(game::OP_SELCET_PET, petId);
	sendPacket(op);
}

void LoongBgClient::onAttack()
{
	BgClientDataMgr::BgClientDataMapT& clientMapsBg = sBgClientDataMgr.getClientMap();
	std::map<int32, BgClientData*>::iterator iter;
	for(iter = clientMapsBg.begin(); iter != clientMapsBg.end(); iter++)
	{
		BgClientData* client = iter->second;
		if (team_ != client->team)
		{
			int32 x = client->x;
			int32 y = client->y;

			int32 deltax = x - x_;
			int32 deltay = y - y_;
			if ( deltax * deltax + deltay * deltay <= 1600)
			{
				attacker_= client->playerId;
				useSkill();
				break;
			}
		}
	}
}

static bool sendPingPacket = true;

void LoongBgClient::onPingHandler()
{
	if (sendPingPacket)
	{
		PacketBase op(game::OP_PING, 0);
		sendPacket(op);
		sTimestamp_ = Timestamp::now();
		sendPingPacket = false;
	}
}

void LoongBgClient::useSkill()
{
	//param  技能ID
	//uintType:int 目标单元 (0 玩家 1 暗黑军王座 2 烈阳军王座)
	//target:int 目标

	int32 skillId = skillList[petId_][getRandomBetween(0, 1)];
	PacketBase op(game::OP_USE_SKILL, skillId);
	op.putInt32(0);
	op.putInt32(attacker_);
	sendPacket(op);
}

void LoongBgClient::onStandHandler()
{
	if (state_ == MOVE_STATE)
	{
		PacketBase op(game::OP_STAND, 0);
		op.putInt32(x_);
		op.putInt32(y_);
		sendPacket(op);
	}
}

void LoongBgClient::sendPacket(PacketBase& pb)
{
	if (pCon_)
	{
		codec_.send(pCon_, pb);
	}
}

void LoongBgClient::onMsgHandler(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	uint32 op = pb.getOP();
	switch (op)
	{
	case client::OP_ENTER_BATTLE:
		onEnterBattle(pCon, pb);
		break;
	case client::OP_SELCET_PET:
		onSelect(pCon, pb);
		break;
	case client::OP_ADD_PLAYER:
		onAddPlayer(pCon, pb);
		break;
	case client::OP_ON_HURT:
		onHurt(pCon, pb);
		break;
	case client::OP_REMOVE_PLAYER:
		onRemovePlayer(pCon, pb);
		break;
	case client::OP_REQ_BATTLE_INFO:
		onReqBattleInfo(pCon, pb);
		break;
	case client::OP_REQ_PLAYER_LIST:
		onReqPlayerList(pCon, pb);
		break;
	case client::OP_PING:
		onPing(pCon, pb, timestamp);
		break;
	case client::OP_MOVE:
		onMove(pCon, pb);
		break;
	default:
		LOG_INFO << name_ << ", op no found, op:" << op;
		break;
	}
}

void LoongBgClient::onEnterBattle(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	int32 param = pb.getParam();
	if (param == 0)
	{
		LOG_INFO << name_ << " 进入成功  ";

		LOG_INFO << name_ << " 发送请求战场基本信息";
		PacketBase op1(game::OP_REQ_BATTLE_INFO, 0);
		codec_.send(pCon, op1);

		LOG_INFO << name_ << " 发送请求战场玩家的列表";
		PacketBase op2(game::OP_REQ_PLAYER_LIST, 0);
		codec_.send(pCon, op2);
	}
	else if (param == -1)
	{
		LOG_INFO << name_ << " 满人 ";
	}
	else if (param == -2)
	{
		LOG_INFO << name_ << " 战场在清理中 ";
	}
}

void LoongBgClient::onSelect(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	//param playerId 玩家id
	//petID:int 英雄ID
	//hp:int 英雄血量
	int32 playerId = pb.getParam();
	int32 petId = pb.getInt32();
	int32 hp = pb.getInt32();
	if (playerId == playerId_) // 自己选择宠物
	{
		petId_= petId;
		hp_ = hp;
		LOG_INFO << "LoongBgClient::onSelect 选英雄-- " << " playerId: " << playerId << " petid: " << petId_ << " hp:" << hp_;
		//skillList_.clear();
		//int32 count = pb.getInt32();
		//for (int32 i = 0; i < count; i++)
		//{
		//	int32 skillId = pb.getInt32();
		//	skillList_.push_back(skillId);
		//}

		state_ = MOVE_STATE;

		if (team_ == 1)
		{
			x_ = blackStartPos[2][0];
			y_ = blackStartPos[2][1];
		}
		else if (team_ == 2)
		{
			x_ = whiteStartPos[2][0];
			y_ = whiteStartPos[2][1];
		}
	}
	else
	{
		BgClientData* bgClient = sBgClientDataMgr.getPlayer(playerId);
		if (bgClient)
		{
			bgClient->petId = petId;
			bgClient->hp = hp;
			if (bgClient->team == 1) //队伍 （1 	为暗黑军 2 为烈阳军）
			{
				bgClient->x = blackStartPos[2][0];
				bgClient->y = blackStartPos[2][1];
			}
			else if (bgClient->team == 2)
			{
				bgClient->x = whiteStartPos[2][0];
				bgClient->y = whiteStartPos[2][1];
			}
		}
	}
}

void LoongBgClient::onAddPlayer(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	LOG_INFO  << " onAddPlayer " ;
	int32 uid = pb.getInt32();
	int32 unitType = pb.getInt32();
    ((void)unitType);
	int32 team = pb.getInt32();
	char name[100];
	if (!pb.getUTF(name, sizeof(name)))
	{
		return;
	}
	BgClientData* bgClient = new BgClientData();
	assert(bgClient);
	bgClient->playerId = uid;
	bgClient->name = name;
	bgClient->team = team;
	if (team == 1) //队伍 （1 	为暗黑军 2 为烈阳军）
	{
		bgClient->x = blackStartPos[1][0];
		bgClient->y = blackStartPos[1][1];
	}
	else if (team == 2)
	{
		bgClient->x = whiteStartPos[1][0];
		bgClient->y = whiteStartPos[1][1];
	}
	sBgClientDataMgr.addPlayer(bgClient);
}

void LoongBgClient::onRemovePlayer(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	//cmd 0x140008
	//param uid 卡布号
	int32 uid = pb.getParam();
	sBgClientDataMgr.removePlayer(uid);
}

void LoongBgClient::onReqBattleInfo(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	//cmd 0x140005
	//param
	//blackNum:int32 暗黑军人数
	//whiteNum:int32 烈阳军人数
	//state:int32 战场状态
	//leftTime:int32 剩余时间 (单位毫秒)
	//blackhp:int32 暗黑军王座血量
	//whitehp:int32 烈阳军血量
	//itemList:array 战场物品列表
	//{
	//itemId:int32 战场物品ID
	//}
}

void LoongBgClient::onReqPlayerList(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	//cmd 0x140009
	//param 战场玩家的人数
	//uid:int32 玩家id
	//unitType:int32 这个客户端暂时可以不用理
	//team:int32 玩家所在队伍
	//name:utf 玩家的名字
	//roletype:int32
	//petId:int 英雄id
	//hp:int 血量
	//mxhp:int 最大血量
	//title:int 称号
	//x:int
	//y:int
	int32 count = pb.getParam();
	for (int32 i = 0; i < count; i++)
	{
		int32 uid = pb.getInt32();
		int32 unitType = pb.getInt32();
		((void)unitType);
		int32 team = pb.getInt32();
		char name[100];
		if (!pb.getUTF(name, sizeof(name)))
		{
			break;
		}
		int32 roleType = pb.getInt32();
		((void)roleType);
		int32 petId = pb.getInt32();
		int32 hp = pb.getInt32();
		int32 maxhp = pb.getInt32();
		int32 title = pb.getInt32();
		((void)title);
		int32 x = pb.getInt32();
		int32 y = pb.getInt32();

		BgClientData* bgClient = new BgClientData();
		assert(bgClient);
		bgClient->playerId = uid;
		bgClient->name = name;
		bgClient->petId = petId;
		bgClient->maxhp = maxhp;
		bgClient->hp = hp;
		bgClient->team = team;
		bgClient->x = x;
		bgClient->y = y;

		sBgClientDataMgr.addPlayer(bgClient);
	}
	sBgClientDataMgr.printInfo();
	state_ = SELECT_STATE;
}

void LoongBgClient::onPing(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
{
	Timestamp now = Timestamp::now();
	double delayTime = timeDifference(now, sTimestamp_);
	LOG_INFO << "[DELAYTIME] " << delayTime << " (s)";
	sTimestamp_ = now;
	sendPingPacket = true;
}

void LoongBgClient::onMove(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	int32 uid = pb.getInt32();
	int32 x = pb.getInt32();
	int32 y = pb.getInt32();
	if (uid == playerId_)
	{
		x_ = x;
		y_ = y;
	}
	else
	{
		BgClientData* bgClient = sBgClientDataMgr.getPlayer(uid);
		if (!bgClient) return;

		bgClient->x = x;
		bgClient->y = y;
	}
}

void LoongBgClient::onHurt(mysdk::net::TcpConnection* pCon, PacketBase& pb)
{
	//param  卡布号
	//unitType:int 单元类型 0 玩家 1 暗黑军王座 2 烈阳军王座
	//damageType:int 伤害类型 0 技能伤害 1 buf 伤害
	//if(damageType == 0)
	//{
	//	skillId:int 技能ID
	//	skillType:int 技能类型 (0 普通攻击 1 技能功能)
	//	damage:int 伤害值
	//}
	//else if (damageType == 1)
	//{
	//	bufId:int bufID
	//	damage:int 伤害值
	//}

	int32 damage = 0;
	int32 attacker;
	int32 playerId = pb.getParam();
	int32 unitType =pb.getInt32();

	int32 damageType = pb.getInt32();
	if (damageType == 0)
	{
		int32 skillId = pb.getInt32();
		int32 skillType = pb.getInt32();
		((void)skillType);
		damage = pb.getInt32();
		LOG_INFO << "onHurt - "<< " playerId: " << playerId
							<< "  skillId: " << skillId
							<< " unitType: " << unitType
							<< " damage: " << damage;
		attacker = pb.getInt32();
	}
	else if (damageType == 1)
	{
		int32 bufId = pb.getInt32();
		damage = pb.getInt32();
		LOG_INFO << "onHurt - "<< " playerId: " << playerId
							<< "  bufId: " << bufId
							<< " unitType: " << unitType
							<< " damage: " << damage;
	}
	if (playerId == playerId_)
	{
		hp_ -= damage;
		if (hp_ <= 0)
		{
			state_ = RESELECT_STATE;
		}
		else // 攻击这个人
		{
			attacker_ = attacker;
			state_ = ATTACK_STATE;
		}
	}
	else
	{
		BgClientData* bgClient = sBgClientDataMgr.getPlayer(playerId);
		if (!bgClient) return;

		bgClient->hp -= damage;
	}
}
