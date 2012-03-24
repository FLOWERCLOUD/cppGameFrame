
#ifndef MYSDK_PROTOCOL_KABU_CODEC_PACKETBASE_H
#define MYSDK_PROTOCOL_KABU_CODEC_PACKETBASE_H

#include <mysdk/base/Common.h>

#include <mysdk/net/Buffer.h>

#include <string>

using namespace std;
using namespace mysdk;
using namespace mysdk::net;

class PacketBase
{
public:
	typedef struct tagKaBuHead_t
	{
		tagKaBuHead_t():
			magic(0),
			len(0),
			op(0),
			param(0)
		{
		}
		uint16		magic;
		uint16 	len;
		uint32 	op;
		uint32	    param;
	} KaBuHead;

public:
	PacketBase(uint16 magic, uint16 len, uint32 op, uint32 param, const char* buf, uint16 bufLen);
	PacketBase(uint16 magic, uint16 len, uint32 op, uint32 param, const string& buf);
	PacketBase(uint32 op, uint32 param);
	PacketBase()
	{

	}
	~PacketBase();

	uint32 getOP();
	uint32 getParam();
	void setOP(uint32 op);
	void setParam(uint32 param);

	int16 getInt16();
	uint32 getInt32();
	bool getUTF(char* pString, int16 nMaxLen);

	bool putInt16(int16 be16);
	bool putInt32(uint32 be32);
	bool putUTF(char* utf, int16 utfLen);
	bool putUTF(std::string utf);
	int16 getContentLen();

	void prepend(const void* data, size_t len);
	void prepend(mysdk::net::Buffer& buf);
	const mysdk::net::Buffer* getBufferContent() const
	{
		return &buffer_;
	}
private:
	KaBuHead head_;
	Buffer buffer_;
private:
	DISALLOW_COPY_AND_ASSIGN(PacketBase);
};

#endif /* PACKETBASE_H_ */
