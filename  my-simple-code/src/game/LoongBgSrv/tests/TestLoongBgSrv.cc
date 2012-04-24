/*
 * TestLoongBgSrv.cc
 *
 *  Created on: 2012-3-19
 *    
 */

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpClient.h>
#include <mysdk/protocol/kabu/codec/KaBuCodec.h>
#include <game/LoongBgSrv/protocol/GameProtocol.h>
#include <game/LoongBgSrv/util/md5.h>
#include <game/LoongBgSrv/LoongBgSrv.h>
#include <game/LoongBgSrv/Util.h>

#include <string.h>

static int blackPlayerId = 84109;
static char blackplayerName[] = "黑法师";
static int whitePlayerId = 85000;
static char whiteplayerName[] = "白巫师";

bool black = true;

int getPlayerId()
{
	if (black)
	{
		return blackPlayerId;
	}
	return whitePlayerId;
}

char* getPlayerName()
{
	if (black)
	{
		return blackplayerName;
	}
	return whiteplayerName;
}

class TestLoongBgClient
{
 public:
	TestLoongBgClient(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(loop),
      client_(loop, listenAddr, "TestLoongBgClient"),
      codec_(
  				std::tr1::bind(&TestLoongBgClient::onKaBuMessage,
  				this,
  				std::tr1::placeholders::_1,
  				std::tr1::placeholders::_2,
  				std::tr1::placeholders::_3)),
  	 testTimes_(0)
	{
		client_.setConnectionCallback(
				std::tr1::bind(&TestLoongBgClient::onConnection, this, std::tr1::placeholders::_1));

		client_.setMessageCallback(std::tr1::bind(&KaBuCodec::onMessage,
				&codec_,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3));

	    loop->runEvery(1.0, std::tr1::bind(&TestLoongBgClient::onTimer, this));
	    //loop->runEvery(0.1, std::tr1::bind(&TestLoongBgClient::onTimer, this));
	}

	void connect()
	{
		client_.connect();
	}

	void onTimer()
	{
		if (pCon_)
		{
			PacketBase op(game::OP_PING, 0);
			codec_.send(pCon_, op);
		}
	}
private:
	void onConnection(mysdk::net::TcpConnection* pCon)
	{
		if (!pCon) return;
		LOG_INFO << pCon->localAddress().toHostPort() << " -> "
				<< pCon->peerAddress().toHostPort() << " is "
				<< (pCon->connected() ? "UP" : "DOWN");

		if (pCon->connected())
		{
			pCon_ = pCon;

			int32 playerId = getPlayerId();//getRandomBetween(1, 100000);

			int32 roleType = getRandomBetween(1, 4);
			int32 times = 0;
			//char token[128];
			char key[] = "9B1492CF6AAE903F63FB7759D3565CD7";
			char tmp[128];
			snprintf(tmp, 127, "%d%s%d%d%s", playerId, getPlayerName(), roleType, times, key);

			char* token = MD5String(tmp);
			PacketBase op(game::OP_LOGIN, 0);
			op.putInt32(playerId);
			op.putUTF(getPlayerName(), static_cast<int16>(strlen(getPlayerName())));
			op.putInt32(roleType);
			op.putInt32(times);
			op.putUTF(token, static_cast<int16>(strlen(token)));
			codec_.send(pCon, op);
			free(token);
		}
	}

	void onEnterBattle(mysdk::net::TcpConnection* pCon, PacketBase& pb)
	{
		//param 0 进入成功 -1 满人 -2 战场在清理中
		//bgid:int 战场ID
		//team:int 队伍
		int32 param = pb.getParam();
		if (param == 0)
		{
			LOG_INFO << getPlayerName() << "进入成功  ";

			PacketBase op(game::OP_SELCET_PET, 2);
			codec_.send(pCon, op);
		}
		else if (param == -1)
		{
			LOG_INFO << getPlayerName() << "满人 ";
		}
		else if (param == -2)
		{
			LOG_INFO << getPlayerName() << "战场在清理中 ";
		}
	}

	void onSelect(mysdk::net::TcpConnection* pCon, PacketBase& pb)
	{
		//param playerId 玩家id
		//petID:int 英雄ID
		//hp:int 英雄血量
		int32 playerId = pb.getParam();
		int32 petid = pb.getInt32();
		int32 hp = pb.getInt32();
		LOG_INFO << getPlayerName() << " playerId: " << playerId << " petid: " << petid << " hp:" << hp;

		PacketBase op(game::OP_USE_SKILL, 4);
		op.putInt32(0);
		op.putInt32(whitePlayerId);
		codec_.send(pCon, op);
	}

	void onAddPlayer(mysdk::net::TcpConnection* pCon, PacketBase& pb)
	{
		LOG_INFO  << " onAddPlayer " ;
		//uid:int32 玩家id
		//unitType:int32 这个客户端暂时可以不用理
		//team:int32 玩家所在队伍
		//name:utf 玩家的名字
		//roletype:int32
		int32 uid = pb.getInt32();
		//int32 unitType =
				pb.getInt32();
		//int32 team =
				pb.getInt32();
		char name[100];
		pb.getUTF(name, 100);
		if (strcmp(name, getPlayerName()) != 0)
		{
			//param  技能ID
			//uintType:int 目标单元 (0 玩家 1 暗黑军王座 2 烈阳军王座)
			//target:int 目标

			PacketBase op(game::OP_USE_SKILL, 4);
			op.putInt32(0);
			op.putInt32(uid);
			codec_.send(pCon, op);
		}
		else
		{
			//LOG_INFO << getPlayerName() << " playerId: " << playerId << " my here!!! ";
		}
	}

	void onHurt(mysdk::net::TcpConnection* pCon, PacketBase& pb)
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

		int32 playerId = pb.getParam();
		//int32 unitType =
				pb.getInt32();
		int32 damageType = pb.getInt32();
		if (damageType == 0)
		{
			int32 skillId = pb.getInt32();
			//int32 skillType =
					pb.getInt32();
			int32 damage = pb.getInt32();
			LOG_INFO << "onHurt - "<< " playerId: " << playerId
								<< "  skillId: " << skillId
								<< " damage: " << damage;
		}
		else if (damageType == 1)
		{
			int32 bufId = pb.getInt32();
			int32 damage = pb.getInt32();
			LOG_INFO << "onHurt - "<< " playerId: " << playerId
								<< "  bufId: " << bufId
								<< " damage: " << damage;
		}
	}

	void onMsgHandler(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
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
		default:
			LOG_INFO << getPlayerName() << " op no found, op:" << op;
			break;
		}
	}

	void onKaBuMessage(mysdk::net::TcpConnection* pCon, PacketBase& pb, mysdk::Timestamp timestamp)
	{
		uint32 op = pb.getOP();
		if (op == client::OP_LOGIN)
		{
			int32 result = pb.getInt32();
			if (result == 0)
			{
				LOG_INFO << getPlayerName() << " 登陆成功";

				PacketBase pb1(game::OP_ENTER_BATTLE, 0);
				//bgid:int 战场ID
				//team:int 队伍 （1 	为暗黑军 2 为烈阳军）
				int32 bgid = 1;
				int32 team = black == true ? 1 : 2;
				pb1.putInt32(bgid);
				pb1.putInt32(team);
				codec_.send(pCon, pb1);
			}
			else if (result == -1)
			{
				LOG_INFO << getPlayerName() << "登陆失败 ";
				pCon->shutdown();
			}
			else if (result == -2)
			{
				LOG_INFO << getPlayerName() << " 已经玩5次啦 不能再玩啦 " ;
				pCon->shutdown();
			}
			else if (result == -3)
			{
				LOG_INFO << getPlayerName() << " 异地登陆";
				pCon->shutdown();
			}
		}
		else
		{
			onMsgHandler(pCon, pb, timestamp);
		}
	}
private:
	  EventLoop* loop_;
	  TcpClient client_;
	  mysdk::net::TcpConnection* pCon_;
	  KaBuCodec codec_;
	  int32 testTimes_;
};


int main(int argc, char* argv[])
{
	  LOG_INFO << "pid = " << getpid();
	  if (argc > 2)
	  {
		  EventLoop loop;
		  InetAddress serverAddr(argv[1], 2007);

		  if (atoi(argv[2]) == 1)
		  {
			  black = false;
		  }

		  TestLoongBgClient client(&loop, serverAddr);
		  client.connect();
		  loop.loop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip\n", argv[0]);
	  }
	return 0;
}
