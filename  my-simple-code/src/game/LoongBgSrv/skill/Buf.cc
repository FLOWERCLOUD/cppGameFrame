/*
 * Buf.cc
 *
 *  Created on: 2012-3-20
 *    
 */

#include <game/LoongBgSrv/skill/Buf.h>

#include <mysdk/base/Logging.h>

Buf::Buf():
	bufId_(0),
	bufName_("buf"),
	waitDel_(false),
	tickTime_(0),
	lastTime_(0),
	bufferTime_(0)
{
}

Buf::Buf(int16 bufId, std::string& bufTypeName):
	bufId_(bufId),
	bufName_(bufTypeName),
	waitDel_(false),
	tickTime_(0),
	lastTime_(0),
	bufferTime_(0)
{
}

Buf::Buf(int16 bufId, std::string& bufTypeName, uint32 curTime, uint32 tickTime, uint32 bufferTime):
	bufId_(bufId),
	bufName_(bufTypeName),
	waitDel_(false),
	tickTime_(tickTime),
	lastTime_(curTime),
	bufferTime_(bufferTime)
{

}

Buf::Buf(int16 bufId, const char* bufTypeName, uint32 curTime, uint32 tickTime, uint32 bufferTime):
	bufId_(bufId),
	bufName_(bufTypeName),
	waitDel_(false),
	tickTime_(tickTime),
	lastTime_(curTime),
	bufferTime_(bufferTime)
{

}


Buf::~Buf()
{
}

void Buf::run(BgUnit* me, uint32 curTime)
{
	LOG_TRACE << " Buf::run - playerId: " << me->getId()
							<< " curTime: " << curTime;

	if (tickTime_ > 0)
	{
		while (curTime >= tickTime_ + lastTime_)
		{
			lastTime_  += tickTime_;
			onTick(me);
		}
	}

	if (bufferTime_ > 0 && curTime >= bufferTime_)
	{
		onDelete(me);
		waitDel_ = true;
	}
}

int16 Buf::getId()
{
	return bufId_;
}

std::string Buf::getName()
{
	return bufName_;
}

void Buf::setId(int16 id)
{
	bufId_ = id;
}

void Buf::setName(std::string& name)
{
	bufName_ = name;
}

void Buf::onDelete(BgUnit* me)
{

}

void Buf::onTick(BgUnit* me)
{

}

void Buf::onCacl(BgUnit* me)
{

}

bool Buf::waitDel()
{
	return waitDel_;
}
