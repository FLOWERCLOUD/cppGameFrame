
#include <game/LoongBgSrv/base/Base.h>
#include <game/LoongBgSrv/config/ConfigMgr.h>
#include <game/LoongBgSrv/php/htmlclient.h>
#include <game/LoongBgSrv/LoongBgSrv.h>

#include <mysdk/net/EventLoop.h>
#include <mysdk/net/InetAddress.h>

#define 	LOONGBGSRVVERSION	0.1

int main()
{
	LOG_INFO << "============ Start LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	const char* config_file = "config/def.conf";
	if(sConfigMgr.MainConfig.SetSource(config_file, true))
	{
		LOG_INFO << "Config Passed without errors.";
	}
	else
	{
		LOG_ERROR << " Config Encountered one or more errors.";
		return -1;
	}

	uint16 srvPort = static_cast<uint16>(sConfigMgr.MainConfig.GetIntDefault("net", "port", 2007));
	std::string host = sConfigMgr.MainConfig.GetStringDefault("mysql", "host", "127.0.0.1");
	std::string port_or_socket = sConfigMgr.MainConfig.GetStringDefault("mysql", "port_or_socket", "3306");
	std::string user = sConfigMgr.MainConfig.GetStringDefault("mysql", "user", "root");
	std::string password = sConfigMgr.MainConfig.GetStringDefault("mysql", "password", "4399mysql#CQPZM");
	std::string database = sConfigMgr.MainConfig.GetStringDefault("mysql", "database", "kabu_loongBg");

	if (!sBase.init(host, port_or_socket, user, password, database))
	{
		LOG_ERROR << " init data base error!!! ";
		return -1;
	}

	CHtmlClient::initialize();
	EventLoop loop;
	InetAddress listenAddr(srvPort);
	LoongBgSrv server(&loop, listenAddr);
	server.start();
	loop.loop();
	server.stop();
	CHtmlClient::finalise();
	LOG_INFO << "============  Stop LoongBgSrv ============ version "  << LOONGBGSRVVERSION;
	return 0;
}
