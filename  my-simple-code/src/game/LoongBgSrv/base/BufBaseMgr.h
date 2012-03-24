/*
 * BufBaseMgr.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_BUFBASEMGR_H_
#define GAME_BUFBASEMGR_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>

#include <game/LoongBgSrv/base/BufBase.h>

class BufBaseMgr
{
public:
	static const int sMaxBufNum = 6;
public:
	BufBaseMgr();
	~BufBaseMgr();

	bool init();
	void shutdown();

	const BufBase& getBufBaseInfo(int16 bufId) const;
private:
	int16 curBufBaseNum_;
	BufBase bufBaseList_[sMaxBufNum ];
};

#define sBufBaseMgr mysdk::Singleton<BufBaseMgr>::instance()

#endif /* BUFBASEMGR_H_ */
