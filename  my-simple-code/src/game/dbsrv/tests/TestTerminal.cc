
#include "ClientThread.h"

#include <stdio.h>
#include <stdlib.h>

#include "linenoise.h"

using namespace mysdk;

void completion(const char *buf, linenoiseCompletions *lc)
{
	char set[] = "set";
	char get[] = "get";
    if (buf[0] == 's')
    {
        linenoiseAddCompletion(lc, set);
    }
    else if (buf[0] == 'g')
    {
        linenoiseAddCompletion(lc, get);
    }
}

int main(int argc, char* argv[])
{
	 Logger::setLogLevel(Logger::WARN);
	  if (argc > 2)
	  {
		  EventLoop loop;
		  short port = static_cast<short>(atoi(argv[2]));
		  InetAddress serverAddr(argv[1], port);

		  ClientThread thread(serverAddr);
		  thread.start();
		  char *line;
		  char historyfilename[] = "history.txt";
		  char propmt[] = "dbsrv> ";
		  linenoiseSetCompletionCallback(completion);
		  linenoiseHistoryLoad(historyfilename); /* Load the history at startup */
		  while((line = linenoise(propmt)) != NULL)
		  {
		        if (line[0] != '\0')
		        {
		            printf("echo: '%s'\n", line);
		            thread.push(line);
		            linenoiseHistoryAdd(line);
		            linenoiseHistorySave(historyfilename); /* Save every new entry */
		        }
		        free(line);
		   }

		  thread.stop();
	  }
	  else
	  {
		  printf("Usage: %s host_ip port\n", argv[0]);
	  }
	return 0;
}
