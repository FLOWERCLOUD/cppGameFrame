/*
 * BgClientData.h
 *
 *  Created on: 2012-4-19
 *    
 */

#ifndef GAME_BGCLIENTDATA_H_
#define GAME_BGCLIENTDATA_H_

#include <mysdk/base/Common.h>

#include <string>

using namespace mysdk;
class BgClientData
{
public:
	BgClientData();
public:
	bool isDead();
	void printInfo();
public:
	int32 playerId;
	std::string name;
	int32 petId;
	int32 maxhp;
	int32 hp;
	int32 team;
	int32 x;
	int32 y;
};

#endif /* BGCLIENTDATA_H_ */
