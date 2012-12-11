
#include <game/dbsrv/tests/DBClient.h>

#include <mysdk/base/Logging.h>
#include "../msg.pb.h"

DBClient::DBClient(EventLoop* loop, const InetAddress& listenAddr):
	loop_(loop),
	client_(loop, listenAddr, "DBClient"),
	pCon_(NULL),
	codec_(
				std::tr1::bind(&DBClient::onKaBuMessage,
				this,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3))
{
	client_.setConnectionCallback(
			std::tr1::bind(&DBClient::onConnection, this, std::tr1::placeholders::_1));

	client_.setMessageCallback(std::tr1::bind(&KabuCodec::onMessage,
			&codec_,
			std::tr1::placeholders::_1,
			std::tr1::placeholders::_2,
			std::tr1::placeholders::_3));
}

void DBClient::connect()
{
	client_.connect();
}

void DBClient::onConnection(mysdk::net::TcpConnection* pCon)
{
	if (!pCon) return;
	//LOG_INFO << pCon->localAddress().toHostPort() << " -> "
	//		<< pCon->peerAddress().toHostPort() << " is "
	//		<< (pCon->connected() ? "UP" : "DOWN");

	if (pCon->connected())
	{
		printf("connection success\n");
		//fflush(stdout);

		pCon_ = pCon;
#if 0
		db_srv::get* get = new db_srv::get();
		get->set_uid(100);
		get->set_argback("test111111");
		get->add_table_name("user");
		get->add_table_name("pet");
		send(get);

		db_srv::set* set = new db_srv::set();
		set->set_uid(100);
		set->set_argback("test11111");
		db_srv::set_table* usertable = set->add_tables();
		usertable->set_table_name("user");
		const char table_bin[] = "123456789\n\n\0\0$$$####@@@__**&&`'''',,,,,\\=-/,\0";
		std::string stmp(table_bin, 38);
		LOG_DEBUG << "stmp: " << stmp.size();
		usertable->set_table_bin(table_bin, 38);

		db_srv::set_table* pettable = set->add_tables();
		pettable->set_table_name("pet");
		pettable->set_table_bin("99999999999\n\n$$$####@@@__**&&`'''',,,,,\\=-/,888888888866666^^%%%");

		LOG_DEBUG << set->DebugString();
		send(set);

#endif
	}
	else
	{
		pCon_ = NULL;
	}
}

void DBClient::onKaBuMessage(mysdk::net::TcpConnection* pCon,
		google::protobuf::Message* message,
		mysdk::Timestamp timestamp)
{
	LOG_INFO << "onKaBuMessage: " << message->GetTypeName()<< message->DebugString();
	//printf("\n===================");
	printf("\nmsg type:%s\n, msg string:%s\n", message->GetTypeName().c_str(),
			message->DebugString().c_str());
	//printf("===================\n");
	//fflush(stdout);
}

void DBClient::send(google::protobuf::Message* message)
{
    codec_.send(pCon_, message);
    // 现在可以释放掉这个消息了
    delete message;
}

void DBClient::sendGetCmd()
{
	db_srv::get* get = new db_srv::get();
	get->set_uid(54689020);
	get->set_argback("test111111");
	get->add_table_name("user");
	get->add_table_name("pet");
	send(get);
}

void DBClient::sendSetCmd()
{
	db_srv::set* set = new db_srv::set();
	set->set_uid(100);
	set->set_argback("test11111");
	db_srv::set_table* usertable = set->add_tables();
	usertable->set_table_name("user");
	const char table_bin[] = "123456789\n\n\0\0$$$####@@@__**&&`'''',,,,,\\=-/,\0";
	std::string stmp(table_bin, 38);
	LOG_DEBUG << "stmp: " << stmp.size();
	usertable->set_table_bin(table_bin, 38);

	db_srv::set_table* pettable = set->add_tables();
	pettable->set_table_name("pet");
	pettable->set_table_bin("99999999999\n\n$$$####@@@__**&&`'''',,,,,\\=-/,888888888866666^^%%%");

	LOG_DEBUG << set->DebugString();
	send(set);
}

void DBClient::sendUnknowCmd()
{
	db_srv::lua* lua = new db_srv::lua();
	lua->set_uid(200);
	lua->set_argback("ggggg");
	LOG_DEBUG << lua->DebugString();
	send(lua);
}
