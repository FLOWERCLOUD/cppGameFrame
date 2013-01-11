
#ifndef GAME_SRVSTATEINFO_H_
#define GAME_SRVSTATEINFO_H_

#include <mysdk/base/Singleton.h>

#include <string>
#include <map>

class SrvStateInfo
{
public:
	SrvStateInfo();
	~SrvStateInfo();

	void start();

	void print();
	void addReqNum();
	void statistics(int sec);
private:
	unsigned int reqNumPer_;
};

#define sSrvStateInfo mysdk::Singleton<SrvStateInfo>::instance()

#endif
