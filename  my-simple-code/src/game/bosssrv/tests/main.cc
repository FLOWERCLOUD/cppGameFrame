/*
 * TestLoongBgSrv.cc
 *
 *  Created on: 2012-3-19
 *    
 */

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include <mysdk/net/TcpClient.h>

#include <game/bosssrv/tests/Client.h>

using namespace mysdk;
using namespace mysdk::net;

int main(int argc, char* argv[])
{
	  LOG_INFO << "pid = " << getpid();
	  if (argc > 2)
	  {
		  EventLoop loop;
		  uint16 port = static_cast<uint16>(atoi(argv[2]));
		  InetAddress serverAddr(argv[1], port);

		  Client client(&loop, serverAddr);
		  client.connect();
		  loop.loop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip host_port \n", argv[0]);
	  }
	return 0;
}
