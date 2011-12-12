
#include <mysdk/net/Session.h>

#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>

#include <sstream>

using namespace mysdk;
using namespace mysdk::net;

const int Session::kNoneEvent = 0x00000000;
const int Session::kReadEvent	=	0x00000001;
const int Session::kWriteEvent	=	0x00000010;
const int Session::kErrorEvent	=	0x00000100;
const int Session::kHupEvent 	=	0x00001000;

Session::Session(EventLoop* loop, int sfd):
		loop_(loop),
		fd_(sfd),
		events_(Session::kNoneEvent),
		fireEvents_(Session::kNoneEvent),
		readCallback_(NULL),
		writeCallback_(NULL),
		closeCallback_(NULL),
		errorCallback_ (NULL)
{
}

Session::~Session()
{
	if (events_ != Session::kNoneEvent)
	{
		disableAll();
	}
}

void Session::enableReading()
{
	loop_->addEvent(this, kReadEvent);
	events_ |= kReadEvent;
}

void Session::enableWriting()
{
	loop_->addEvent(this, kWriteEvent);
	events_ |= kWriteEvent;
}

void Session::disableWriting()
{
	loop_->delEvent(this, kWriteEvent);
	events_ &= ~kWriteEvent;
}

void Session::disableAll()
{
	loop_->delEvent(this, kReadEvent | kWriteEvent);
	events_ = kNoneEvent;
}

bool Session::isWriting() const
{
	return events_ & kWriteEvent;
}

void Session::handleEvent(Timestamp receiveTime)
{
	  if ((fireEvents_ & Session::kHupEvent) && !(fireEvents_ & Session::kReadEvent))
	  {
		  LOG_WARN << "Session::handle_event() HUP" << fd();
		  if (closeCallback_) closeCallback_();
	  }
	  else
	  {
			if (fireEvents_ & Session::kErrorEvent)
			{
				LOG_TRACE << "In error Event " << fd();
				if (errorCallback_) errorCallback_();
			}

			if (fireEvents_ & Session::kWriteEvent)
			{
				LOG_TRACE << " In write Event " << fd();
				if (writeCallback_) writeCallback_();
			}

			if (fireEvents_& Session::kReadEvent)
			{
				LOG_TRACE << " In read Event " << fd();
				if (readCallback_) readCallback_(receiveTime);
			}
	  }
}

std::string Session::reventsToString() const
{
  std::ostringstream oss;
  oss << fd_ << ": ";
  if (fireEvents_ & Session::kReadEvent)
    oss << "IN ";
  if (fireEvents_ & Session::kWriteEvent)
    oss << "OUT ";
  if (fireEvents_ & Session::kHupEvent)
    oss << "HUP ";
  if (fireEvents_ & Session::kErrorEvent)
    oss << "ERR ";

  return oss.str().c_str();
}
