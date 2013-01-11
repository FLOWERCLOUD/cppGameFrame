
#include "SrvStateInfo.h"

#include "LogThread.h"
#include "WorkerThreadPool.h"
#include "WriterThreadPool.h"

SrvStateInfo::SrvStateInfo():
reqNumPer_(0)
{
}

SrvStateInfo::~SrvStateInfo()
{

}

void SrvStateInfo::start()
{

}

void SrvStateInfo::print()
{

}

void SrvStateInfo::addReqNum()
{
	reqNumPer_++;
}

void SrvStateInfo::statistics(int sec)
{
	if (sec <= 0) return;

	LOGEX_INFO("[SRVINFO] req total num: %4.2f", reqNumPer_ / sec);
	LOGEX_INFO("[SRVINFO] wirter thread queue info: %s", sWriterThreadPool.getQueueInfo().c_str());
	LOGEX_INFO("[SRVINFO] work thread queue info: %s", sWorkThreadPool.getQueueInfo().c_str());
	reqNumPer_ = 0;
}
