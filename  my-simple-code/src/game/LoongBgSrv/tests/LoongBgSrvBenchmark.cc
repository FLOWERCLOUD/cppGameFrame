

#include <game/LoongBgSrv/tests/LoongBgClient.h>

using namespace mysdk;
int main(int argc, char* argv[])
{
	  LOG_INFO << "pid = " << getpid();
	  if (argc > 8)
	  {
		  char* ip = argv[1];
		  int16 port = static_cast<int16>(atoi(argv[2]));
		  int32 playerId = atoi(argv[3]);
		  string name(argv[4]);
		  int32 roleType = atoi(argv[5]);
		  int32 times = atoi(argv[6]);
		  int32 bgId = atoi(argv[7]);
		  int32 team = atoi(argv[8]);

		  EventLoop loop;
		  InetAddress serverAddr(ip, port);
		  LoongBgClient client(&loop, serverAddr,
				  playerId, name, roleType, times, bgId, team);

		  client.connect();
		  loop.loop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip port\n", argv[0]);
	  }
	return 0;
}

