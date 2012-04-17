
#include <mysdk/protocol/kabu/codec/PacketBase.h>

#include <mysdk/base/Logging.h>

PacketBase::PacketBase(uint16 magic,
		uint16 len,
		uint32 op,
		uint32 param,
		const char* buf,
		uint16 bufLen)
{
	head_.magic = magic;
	head_.len = len;
	head_.op = op;
	head_.param = param;
	buffer_.append(buf, bufLen);

}

PacketBase::PacketBase(uint16 magic, uint16 len, uint32 op, uint32 param, const string& buf)
{
	head_.magic = magic;
	head_.len = len;
	head_.op = op;
	head_.param = param;
	buffer_.append(buf);
}

PacketBase::PacketBase(uint32 op, uint32 param)
{
	head_.magic = 0;
	head_.len = 0;
	head_.op = op;
	head_.param = param;
}

PacketBase::~PacketBase()
{
	head_.magic = 0;
	head_.len = 0;
	head_.op = 0;
	head_.param = 0;
}

uint32 PacketBase::getOP()
{
	return head_.op;
}

uint32 PacketBase::getParam()
{
	return head_.param;
}

void PacketBase::setOP(uint32 op)
{
	head_.op = op;
}

void PacketBase::setParam(uint32 param)
{
	head_.param = param;
}

uint32 PacketBase::getInt32()
{
	if (buffer_.readableBytes() >= sizeof(int32))
	{
		return buffer_.readInt32();
	}
	return 0;
}

int16 PacketBase::getInt16()
{
	if (buffer_.readableBytes() >= sizeof(int16))
	{
		return buffer_.readInt16();
	}
	return 0;
}

bool PacketBase::getUTF(char* pString, int16 nMaxLen)
{
	if (buffer_.readableBytes() < sizeof(int16))
	{
		LOG_ERROR << "PacketBase::getUTF - " << buffer_.readableBytes();
		return false;
	}

	int16 len = buffer_.readInt16();
	if (len >= nMaxLen || static_cast<int16>(buffer_.readableBytes()) < len)
	{
		LOG_ERROR << "PacketBase::getUTF -  buffer readableBytes: " << buffer_.readableBytes() << " len: " << len;
		return false;
	}

	buffer_.get(pString, len);
	buffer_.retrieve(len);
	pString[len] = 0;
	return true;
}

bool PacketBase::putInt16(int16 be16)
{
	buffer_.appendInt16(be16);
	return true;
}

bool PacketBase::putInt32(uint32 be32)
{
	buffer_.appendInt32(be32);
	return true;
}

bool PacketBase::putUTF(char* utf, int16 utfLen)
{
	buffer_.appendInt16(utfLen);
	buffer_.append(utf, utfLen);
	return true;
}

bool PacketBase::putUTF(std::string utf)
{
	int16 utfLen = static_cast<int16>(utf.size());
	buffer_.appendInt16(utfLen);
	buffer_.append(utf);
	return true;
}

int16 PacketBase::getContentLen()
{
	return static_cast<int16>(buffer_.readableBytes());
}

void PacketBase::prepend(const void* data, size_t len)
{
	buffer_.prepend(data, len);
}

void PacketBase::prepend(mysdk::net::Buffer& buf)
{
	buffer_.prepend(buf.peek(), buf.readableBytes());
}

size_t PacketBase::prependableBytes()
{
	return buffer_.prependableBytes();
}

std::string PacketBase::getHeadInfo()
{
	char headInfo[56];
	snprintf(headInfo, sizeof(headInfo), "len: %d, param: %d, hexop: 0x%X", getContentLen(), head_.param, head_.len);
	return std::string(headInfo);
}
