/*
 * SkillBaseMgr.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_SKILLBASEMGR_H_
#define GAME_SKILLBASEMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <game/LoongBgSrv/base/Base.h>
#include <game/LoongBgSrv/base/SkillBase.h>

class SkillBaseMgr
{
public:
	static const int sMaxSkillNum = 10;
public:
	SkillBaseMgr();
	~SkillBaseMgr();

	bool init();
	bool init(TestDatabaseWorkerPool& databaseWorkPool);

	void shutdown();

	const SkillBase& getSkillBaseInfo(int16 skillId) const;
	bool checkSkillId(int16 skillId);
private:
	int16 curSkillBaseNum_;
	SkillBase skillBaseList_[sMaxSkillNum ];
private:
	DISALLOW_COPY_AND_ASSIGN(SkillBaseMgr);
};

#define sSkillBaseMgr mysdk::Singleton<SkillBaseMgr>::instance()

#endif /* SKILLBASEMGR_H_ */
