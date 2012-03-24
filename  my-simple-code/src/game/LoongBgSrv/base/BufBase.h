/*
 * BufBase.h
 *
 *  Created on: 2012-3-21
 *    
 */

#ifndef GAME_BUFBASE_H_
#define GAME_BUFBASE_H_

#include <mysdk/base/Common.h>

#include <string>
using namespace mysdk;
class BufBase
{
public:
	enum
	{
		OTHER 	= 0,
		ME			= 1,
	};
public:
	static const int sMaxBufParamNum = 10;
public:
	BufBase();
	~BufBase();

public:
	int16 id_; //bufID
	std::string name_; //buf 名字
	int16 type_; // buf 类型
	int16 who_; //这个buf 附加给谁
	int16 paramNum_;
	int16 paramList_[sMaxBufParamNum];
};

#endif /* BUFBASE_H_ */
