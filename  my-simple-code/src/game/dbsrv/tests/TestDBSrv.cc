
#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>
#include "DBClient.h"
using namespace mysdk;

int main(int argc, char* argv[])
{
	  if (argc > 1)
	  {
		  EventLoop loop;
		  InetAddress serverAddr(argv[1], 5007);

		  DBClient client(&loop, serverAddr);
		  client.connect();
		  loop.loop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip\n", argv[0]);
	  }
	return 0;
}
