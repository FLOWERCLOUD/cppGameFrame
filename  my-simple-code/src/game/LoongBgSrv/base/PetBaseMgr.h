/*
 * PetBaseMgr.h
 *
 *  Created on: 2012-3-20
 *    
 */

#ifndef GAME_PETBASEMGR_H_
#define GAME_PETBASEMGR_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/base/Base.h>
#include <game/LoongBgSrv/base/PetBase.h>

class PetBaseMgr
{
public:
	static const int sMaxPetId = 10;
public:
	PetBaseMgr();
	~PetBaseMgr();

	bool init();
	bool init(TestDatabaseWorkerPool& databaseWorkPool);
	void shutdown();

	bool checkPetId(int16 petId);
	const PetBase& getPetBaseInfo(int16 petId) const;
private:
	int16 curPetBaseNum_;
	PetBase petBaseList_[sMaxPetId ];
private:
	DISALLOW_COPY_AND_ASSIGN(PetBaseMgr);
};

#define sPetBaseMgr mysdk::Singleton<PetBaseMgr>::instance()

#endif /* PETBASEMGR_H_ */
