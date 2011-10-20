
#ifndef MYSDK_NET_INETADDRESS_H
#define MYSDK_NET_INETADDRESS_H

#include <mysdk/base/Common.h>

#include <netinet/in.h>

#include <string>

namespace mysdk
{
namespace net
{
	class InetAddress
	{
	public:
		explicit InetAddress(uint16 port);
		InetAddress(const std::string& ip, uint16 port);
		InetAddress(const struct sockaddr_in& addr):
			addr_(addr)
		{}

		// default copy/assignment are Okay

		std::string toHostPort() const;

		const struct sockaddr_in& getSockAddrInet() const { return addr_; }
		void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }
	private:
		struct sockaddr_in addr_;
	};

}
}

#endif
