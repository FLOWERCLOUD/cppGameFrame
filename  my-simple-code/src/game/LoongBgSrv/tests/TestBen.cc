
#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <stdlib.h>

using namespace mysdk;
int main(int argc, char* argv[])
{
	  LOG_INFO << "pid = " << getpid();
	  if (argc > 4)
	  {
		  char* ip = argv[1];
		  int16 port = static_cast<int16>(atoi(argv[2]));
		  int num = atoi(argv[3]);
		  char cmd[1024];
		  char exe[] = "./bin/LoongBgSrvBenchmark";
		  int bgId = atoi(argv[4]);
		  for (int i = 1; i <= num; i++)
		  {
			  //char* ip = argv[1];
			  //int16 port = static_cast<int16>(atoi(argv[2]));
			  //int32 playerId = atoi(argv[3]);
			  //string name(argv[4]);
			  //int32 roleType = atoi(argv[5]);
			  //int32 times = atoi(argv[6]);
			  //int32 bgId = atoi(argv[7]);
			  //int32 team = atoi(argv[8]);
			  snprintf(cmd, sizeof(cmd), "%s %s %d %d %d %d %d %d %d &", exe, ip, port, i, i, 1, 0, bgId, i%2);
			  system(cmd);
			  if (i % 8 == 0)
			  {
				  bgId++;
			  }
		  }
	  }
	  else
	  {
		  printf("Usage: %s num \n", argv[0]);
	  }
	return 0;
}
