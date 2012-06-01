/*
 * BossHurtMgr.h
 *
 *  Created on: 2012-5-30
 *    
 */

#ifndef GAME_BOSSHURTMGR_H_
#define GAME_BOSSHURTMGR_H_

#include <mysdk/base/Common.h>

#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <string>
#include <map>

using namespace mysdk;

class Player
{
public:
	uint32 uid;
	uint32 hurtvalue;
	std::string username;

    bool   operator <  (const   Player&   rhs)  const   //升序排序时必须写的函数
   {
      return   hurtvalue   <   rhs.hurtvalue;
   }

    bool   operator >  (const   Player&   rhs   )  const   //降序排序时必须写的函数
   {
       return   hurtvalue   >   rhs.hurtvalue;
    }
};

class BossSrv;
class BossHurtMgr
{
public:
	typedef std::map<uint32, uint32> PlayerHurtMapT;
	typedef std::vector<Player*> TopPlayerVectorT;

	static const uint32 sMaxTopNum = 10; //最大排行榜人数
public:
	BossHurtMgr(BossSrv* bossSrv);
	~BossHurtMgr();

	void addHurt(uint32 uid, uint32 hurtValue, char* username);

	bool serializeTop(PacketBase& op);

	PlayerHurtMapT& getPlayerHurtMap()
	{
		return playerMap_;
	}
	TopPlayerVectorT& getTopPlayerVector()
	{
		return topPlayerVector_;
	}
private:
	uint32 getHurtValue(uint32 uid);
	// 添加到排行榜中
	void addTop(uint32 uid, uint32 hurtValue, char* username);
private:
	PlayerHurtMapT playerMap_;
	TopPlayerVectorT topPlayerVector_;
	BossSrv* bossSrv_;
private:
	DISALLOW_COPY_AND_ASSIGN(BossHurtMgr);
};

#endif /* BOSSHURTMGR_H_ */

