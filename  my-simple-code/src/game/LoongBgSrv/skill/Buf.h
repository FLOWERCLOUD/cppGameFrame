/*
 * Buf.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_BUF_H_
#define GAME_BUF_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BgUnit.h>
#include <string>

using namespace mysdk;

class Buf
{
public:
	Buf();
	Buf(int16 bufId, std::string& bufTypeName);
	Buf(int16 bufId, std::string& bufTypeName, uint32 curTime, uint32 tickTime, uint32 bufferTime);
	Buf(int16 bufId, const char* bufTypeName, uint32 curTime, uint32 tickTime, uint32 bufferTime);
	virtual ~Buf();

	void run(BgUnit* me, uint32 curTime);

	int16 getId();
	std::string getName();
	void setId(int16 id);
	void setName(std::string& name);

	bool waitDel();
public:
	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);

protected:
	int16 bufId_; // Buffer ID
	std::string bufName_; // 名称

	bool waitDel_; //等待被移除的buf
	uint32 tickTime_;  // buf 多少秒tick 一下
	uint32 lastTime_;
	uint32 bufferTime_; // buf 持续时间
};

#endif /* SKILL_H_ */
