
#include <mysdk/net/Buffer.h>

#include <mysdk/net/SocketsOps.h>


#include <errno.h>
#include <string.h>
#include <sys/uio.h>

using namespace mysdk;
using namespace mysdk::net;

const char Buffer::kCRLF[] = "\r\n";

void Buffer::appendInt32(int32 x)
{
  int32 be32 = sockets::hostToNetwork32(x);
  append(&be32, sizeof be32);
}

int32 Buffer::peekInt32() const
{
  //assert(readableBytes() >= sizeof(int32));
  int32 be32 = 0;
  ::memcpy(&be32, peek(), sizeof be32);
  return sockets::networkToHost32(be32);
}

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
	char extrabuf[65536];
	struct iovec vec[2];
	size_t writeable = writeableBytes();
	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writeable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);
	ssize_t n = readv(fd, vec, 2);
	if ( n < 0)
	{
		*savedErrno = errno;
	}
	else if (implicit_cast<size_t>(n) <= writeable)
	{
		writerIndex_ += n;
	}
	else
	{
		writerIndex_ = buffer_.size();
		append(extrabuf, n - writeable);
	}
	  return n;
}
