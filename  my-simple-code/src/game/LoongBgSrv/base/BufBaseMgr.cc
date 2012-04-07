/*
 * BufBaseMgr.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/base/BufBaseMgr.h>

BufBaseMgr::BufBaseMgr():
	curBufBaseNum_(0)
{
	//init();
}

BufBaseMgr::~BufBaseMgr()
{

}

bool BufBaseMgr::init()
{
	bufBaseList_[curBufBaseNum_].id_ = 1;
	bufBaseList_[curBufBaseNum_].name_ = "晕眩";
	bufBaseList_[curBufBaseNum_].type_ = 1;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 1;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 3000; //单元ms
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 2;
	bufBaseList_[curBufBaseNum_].name_ = "灼伤";
	bufBaseList_[curBufBaseNum_].type_ = 2;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 2;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 5;
	bufBaseList_[curBufBaseNum_].paramList_[1] = 15000; //单元ms
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 3;
	bufBaseList_[curBufBaseNum_].name_ = "溅射";
	bufBaseList_[curBufBaseNum_].type_ = 3;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 4;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 10;
	bufBaseList_[curBufBaseNum_].paramList_[1] = 100;
	bufBaseList_[curBufBaseNum_].paramList_[2] = 150;
	bufBaseList_[curBufBaseNum_].paramList_[3] = 30;
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 4;
	bufBaseList_[curBufBaseNum_].name_ = "免疫";
	bufBaseList_[curBufBaseNum_].type_ = 4;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::ME;
	bufBaseList_[curBufBaseNum_].paramNum_ = 1;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 6000; //单元ms

	return true;
}

bool BufBaseMgr::init(TestDatabaseWorkerPool& databaseWorkPool)
{
	const char* sql = "select buffid, buffname, bufftype, buffwho, buffparam from buff";
	ResultSet* res = databaseWorkPool.query(sql);
	if (!res) return false;

	while (res->nextRow())
	{
		const Field* field = res->fetch();
		bufBaseList_[curBufBaseNum_].id_ =  field[0].getInt16();
		bufBaseList_[curBufBaseNum_].name_ = field[1].getString();
		bufBaseList_[curBufBaseNum_].type_ =  field[2].getInt16();
		bufBaseList_[curBufBaseNum_].who_ = field[3].getInt16();
		ColonBuf buf(field[4].getCString());
		int16 num = bufBaseList_[curBufBaseNum_].paramNum_ = buf.GetShort();
		for (int32 i = 0; i < num; i++)
		{
			bufBaseList_[curBufBaseNum_].paramList_[i] = buf.GetShort();
		}
		curBufBaseNum_++;
	}
	delete res;

	return true;
}

void BufBaseMgr::shutdown()
{

}

const BufBase& BufBaseMgr::getBufBaseInfo(int16 bufId) const
{
	static BufBase null;
	if (bufId <= 0 || bufId > curBufBaseNum_) return null;

	return bufBaseList_[bufId - 1];
}
