
#include "ClientThread.h"


ClientThread::ClientThread(InetAddress addr):
	thread_(std::tr1::bind(&ClientThread::threadHandler, this), "clientThread"),
	client_(&loop_, addr)
{
}

ClientThread::~ClientThread()
{
}

void ClientThread::start()
{
	thread_.start();
	client_.connect();
}

void ClientThread::stop()
{
	thread_.join();
}

void ClientThread::threadHandler()
{
	loop_.loop();
}

void ClientThread::push(char* cmd)
{
	if (strcasecmp(cmd, "get") == 0)
	{
		loop_.queueInLoop(std::tr1::bind(&DBClient::sendGetCmd, &client_));
	}
	else if (strcasecmp(cmd, "set") == 0)
	{
		loop_.queueInLoop(std::tr1::bind(&DBClient::sendSetCmd, &client_));
	}
	else if (strcasecmp(cmd, "lua") == 0)
	{
		loop_.queueInLoop(std::tr1::bind(&DBClient::sendUnknowCmd, &client_));
	}
	else
	{
		printf("cmd not know, cmd: %s\n", cmd);
	}
}
