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
	Buf(int16 bufId, std::string& bufTypeName, int64 curTime, uint32 tickTime, int64 bufferTime);
	Buf(int16 bufId, const char* bufTypeName, int64 curTime, uint32 tickTime, int64 bufferTime);
	virtual ~Buf();

	void run(BgUnit* me, int64 curTime);

	int16 getId();
	std::string getName();
	void setId(int16 id);
	void setName(std::string& name);

	bool waitDel();

	void setLastTime(int64 lastTime)
	{
		lastTime_ = lastTime;
	}
	void setBufferTime(int64 bufferTime)
	{
		bufferTime_ = bufferTime;
	}
public:
	virtual void onDelete(BgUnit* me);
	virtual void onTick(BgUnit* me);
	virtual void onCacl(BgUnit* me);

protected:
	int16 bufId_; // Buffer ID
	std::string bufName_; // 名称

	bool waitDel_; //等待被移除的buf
	uint32 tickTime_;  // buf 多少秒tick 一下
	int64 lastTime_;
	int64 bufferTime_; // buf 持续时间
};

#endif /* SKILL_H_ */
