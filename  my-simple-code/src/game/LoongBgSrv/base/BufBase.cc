/*
 * BufBase.cc
 *
 *  Created on: 2012-3-21
 *    
 */

#include <game/LoongBgSrv/base/BufBase.h>

BufBase::BufBase():
	id_(0),
	type_(0),
	who_(0),
	paramNum_(0)
{
	for (int i = 0; i < sMaxBufParamNum; i++)
	{
		paramList_[i] = 0;
	}
}

BufBase::~BufBase()
{
}
