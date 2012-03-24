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

}

BufBaseMgr::~BufBaseMgr()
{

}

bool BufBaseMgr::init()
{
	bufBaseList_[curBufBaseNum_].id_ = 1;
	bufBaseList_[curBufBaseNum_].name_ = "溅射";
	bufBaseList_[curBufBaseNum_].type_ = 1;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 4;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 10;
	bufBaseList_[curBufBaseNum_].paramList_[1] = 100;
	bufBaseList_[curBufBaseNum_].paramList_[2] = 150;
	bufBaseList_[curBufBaseNum_].paramList_[3] = 30;
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 2;
	bufBaseList_[curBufBaseNum_].name_ = "晕眩";
	bufBaseList_[curBufBaseNum_].type_ = 2;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 1;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 3;
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 3;
	bufBaseList_[curBufBaseNum_].name_ = "灼伤";
	bufBaseList_[curBufBaseNum_].type_ = 3;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::OTHER;
	bufBaseList_[curBufBaseNum_].paramNum_ = 2;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 5;
	bufBaseList_[curBufBaseNum_].paramList_[1] = 15;
	curBufBaseNum_++;
	bufBaseList_[curBufBaseNum_].id_ = 4;
	bufBaseList_[curBufBaseNum_].name_ = "免疫";
	bufBaseList_[curBufBaseNum_].type_ = 4;
	bufBaseList_[curBufBaseNum_].who_ = BufBase::ME;
	bufBaseList_[curBufBaseNum_].paramNum_ = 1;
	bufBaseList_[curBufBaseNum_].paramList_[0] = 6;

	return true;
}

void BufBaseMgr::shutdown()
{

}

const BufBase& BufBaseMgr::getBufBaseInfo(int16 bufId) const
{
	static BufBase null;
	if (bufId <= 0 || bufId > sMaxBufNum) return null;

	return bufBaseList_[bufId - 1];
}
