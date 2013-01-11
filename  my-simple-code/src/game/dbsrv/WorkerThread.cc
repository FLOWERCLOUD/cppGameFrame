
#include <game/dbsrv/WorkerThread.h>

#include <game/dbsrv/DBSrv.h>
#include <game/dbsrv/codec/codec.h>
#include <game/dbsrv/config/ConfigMgr.h>
#include <game/dbsrv/lua/LuaMyLibs.h>
#include <game/dbsrv/mysql/QueryResult.h>
#include <game/dbsrv/LogThread.h>
#include <game/dbsrv/PBSql.h>

#include <google/protobuf/message.h>

WorkerThread::WorkerThread(DBSrv* srv, int id):
	id_(id),
	nextWriterThreadId_(0),
	thread_(std::tr1::bind(&WorkerThread::threadHandler, this), "workerThread"),
	srv_(srv),
	dispatcher_(
			std::tr1::bind(&WorkerThread::onLuaMessage,
					this,
					std::tr1::placeholders::_1,
					std::tr1::placeholders::_2))
{
    dispatcher_.registerMessageCallback<db_srv::get>(
        std::tr1::bind(&WorkerThread::onGet,
        		this,
        		std::tr1::placeholders::_1,
        		std::tr1::placeholders::_2));

    dispatcher_.registerMessageCallback<db_srv::set>(
        std::tr1::bind(&WorkerThread::onSet,
        		this,
        		std::tr1::placeholders::_1,
        		std::tr1::placeholders::_2));

    dispatcher_.registerMessageCallback<db_srv::mget>(
        std::tr1::bind(&WorkerThread::onMGet,
        		this,
        		std::tr1::placeholders::_1,
        		std::tr1::placeholders::_2));

	readis_.Init();
}

WorkerThread::~WorkerThread()
{
}

#include "Util.h"

void WorkerThread::start()
{
	// redis
	std::string redisAddr = sConfigMgr.MainConfig.GetStringDefault("redis", "addr", "127.0.0.1");;
	int redisPort = sConfigMgr.MainConfig.GetIntDefault("redis", "port", 6379) ;
	std::string pas = sConfigMgr.MainConfig.GetStringDefault("redis", "password", "");
	int dbid = sConfigMgr.MainConfig.GetIntDefault("redis", "dbid", 0) ;
	readis_.SetServerAddr(redisAddr, redisPort, pas, dbid);
	readis_.ReConnect();
	// mysql
	std::string host = sConfigMgr.MainConfig.GetStringDefault("mysql", "host", "192.168.1.6");
	std::string port_or_socket = sConfigMgr.MainConfig.GetStringDefault("mysql", "port_or_socket", "3306");
	std::string user = sConfigMgr.MainConfig.GetStringDefault("mysql", "user", "root");
	std::string password = sConfigMgr.MainConfig.GetStringDefault("mysql", "password", "4399mysql#CQPZM");
	std::string database = sConfigMgr.MainConfig.GetStringDefault("mysql", "database", "newkabu");
	MySQLConnectionInfo coninfo(host, port_or_socket, user, password, database);
	mysql_.setConnectionInfo(coninfo);

	mysql_.open();
	thread_.start();
	// 打开lua的基本函数库
	luaEngine_.openlibs();
	// 导入自定义的函数库
	lua_State* L = luaEngine_.getLuaState();
	LuaMyLibs::openmylibs(L);

	registerGlobalLua();
	reloadLua();
}

void WorkerThread::reloadLua()
{
	std::string filenames = sConfigMgr.MainConfig.GetStringDefault("lua", "filelist", "server.lua");

	std::vector<std::string> vec = StrSplit(filenames, ",");
	for (size_t i = 0; i < vec.size(); i++)
	{
		luaEngine_.dofile(vec[i].c_str());
	}
}

bool WorkerThread::registerGlobalLua()
{
	lua_State* L = luaEngine_.getLuaState();

	lua_pushlightuserdata(L, this);
	lua_setglobal(L, "sThread");

	lua_pushlightuserdata(L, &this->readis_);
	lua_setglobal(L, "sRedis");

	lua_pushlightuserdata(L, &this->mysql_);
	lua_setglobal(L, "sMySql");

	return true;
}

void WorkerThread::stop()
{
	ThreadParam param;
	param.Type = STOP;
	queue_.put(param);
	thread_.join();
	readis_.Shutdown();
	mysql_.close();
}

void WorkerThread::push(struct ThreadParam& param)
{
	queue_.put(param);
}

size_t WorkerThread::getQueueSize()
{
	return queue_.size();
}

void WorkerThread::threadHandler()
{
	LOGEX_INFO("WorkerThread::threadHandler start... id[%d]", id_ );
	while (true)
	{
		ThreadParam param = queue_.take();
		if (param.Type == PING)
		{
			readis_.Ping();
			mysql_.ping();
		}
		else if (param.Type == CMD)
		{
			handler(param);
		}
		else if (param.Type == STOP)
		{
			break;
		}
	}
	LOGEX_INFO("WorkerThread::threadHandler stop... id[%d]", id_ );
}

void WorkerThread::handler(struct ThreadParam& param)
{
	int conId = param.conId;
	google::protobuf::Message* message = static_cast<google::protobuf::Message*>(param.msg);
	LOGEX_DEBUG("%d, %s, %s", conId, message->GetTypeName().c_str(), message->DebugString().c_str());
	dispatcher_.onProtobufMessage(conId, message);
}

bool WorkerThread::loadFromRedis(int uid, const std::string& tablename, ::db_srv::get_reply_table* table)
{
	bool result = false;
	char rediskey[100];
	snprintf(rediskey, 99, "%s:%d", tablename.c_str(), uid);
	redisReply* reply = static_cast<redisReply*>(readis_.RedisCommand("GET %s", rediskey));
	if (reply && reply->type == REDIS_REPLY_STRING)
	{
		table->set_table_name(tablename);
		table->set_table_bin(reply->str, reply->len);
		table->set_table_status(2); //从redis拿的数据

		result = true;
	}
	readis_.FreeReplyObject(reply);
	return result;
}

// 是否要把这个表的数据解出来
bool WorkerThread::isParseTable(const std::string& tablename, std::string& outTypeName)
{
	if(sConfigMgr.MainConfig.GetString("parsetable", tablename.c_str(), &outTypeName))
	{
		return true;
	}
	return false;
}

int WorkerThread::loadFromMySql(int uid, const std::string& tablename, ::db_srv::get_reply_table* table)
{
	if (!mysql_)
	{
		return -3; // 数据库连不上了
	}

	std::string typeName;
	if (isParseTable(tablename, typeName))
	{
		google::protobuf::Message* msg = PBSql::select(tablename, typeName, uid, mysql_);
		if (!msg)
		{
			return -2; // 创建proto 失败或者从数据库中load数据失败
		}

		std::string table_bin;
		if (!msg->SerializeToString(&table_bin))
		{
			delete msg;
			return -1;// 序列化出错啦
		}

		table->set_table_name(tablename);
		table->set_table_bin(table_bin.c_str(), table_bin.length());
		table->set_table_status(3); // 从数据库拿到的数据

		delete msg;
		return 0;
	}

	char sql[1024];
	// 该类型的表 有多少张
	int tablenum = sConfigMgr.MainConfig.GetIntDefault("table", tablename.c_str(), 1);
	if (tablenum == 1)
	{
		snprintf(sql, 1023, "select table_bin from %s where uid=%d", tablename.c_str(), uid);
	}
	else
	{
		snprintf(sql, 1023, "select table_bin from %s_%d where uid=%d", tablename.c_str(), uid % tablenum, uid);
	}

    MYSQL_RES *result = NULL;
    MYSQL_FIELD *fields = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;
    bool ret = mysql_.query(sql, &result, &fields, &rowCount, &fieldCount);
    if (!ret || !result)
    {
    	return -3; //数据库连不上了
    }

	//  没有这个玩家的记录哦
	if (rowCount == 0)
	{
		// 数据库没有这个玩家的记录
		table->set_table_name(tablename);
		table->set_table_bin("");
		table->set_table_status(1); // 数据库没有这个玩家的记录

		mysql_free_result(result);
		return 0;
	}

	if (rowCount > 1)
	{
		LOGEX_ERROR("loadFromMySql have more row, sql: %s", sql);
	}
    for (uint64 i = 0; i < rowCount; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row) continue;

        ulong* fieldLength = mysql_fetch_lengths(result);
        for (uint32 j = 0; j < fieldCount; j++)
        {
        	table->set_table_status(3); // 从数据库拿到的数据
        	unsigned int  len = static_cast<unsigned int>(fieldLength[j]);
            if (len == 0)
            {
            	table->set_table_bin("");
            }
            else
            {
            	table->set_table_bin(row[j], len);
            }
        }
		// 找到一行记录以后 马上跳出吧 不可能有多行记录的
		break;
    }
    mysql_free_result(result);
	return 0;
}

void WorkerThread::dispatchWriterThread(WriterThreadParam& param)
{
	if (!srv_) return;

	nextWriterThreadId_++;
	if (nextWriterThreadId_ >= sWriterThreadPool.getThreadNum())
	{
		nextWriterThreadId_ = 0;
	}
	sWriterThreadPool.push(nextWriterThreadId_, param);
}

void WorkerThread::sendReply(int conId, google::protobuf::Message* message)
{
	if (!srv_) return;
	EventLoop* loop = srv_->getEventLoop();
	if (!loop) return;

	loop->queueInLoop(std::tr1::bind(&DBSrv::sendReply, srv_, conId, message));
}

void WorkerThread::sendReplyEx(int conId, google::protobuf::Message& message)
{
	if (!srv_) return;
	EventLoop* loop = srv_->getEventLoop();
	if (!loop) return;

	Buffer* pBuf = new Buffer();
	KabuCodec::fillEmptyBuff(pBuf, &message);
	loop->queueInLoop(std::tr1::bind(&DBSrv::sendReplyEx, srv_, conId, pBuf));
}

void WorkerThread::startTime(double delay, const std::string funname, bool bOne)
{
}

void WorkerThread::_startTime(const std::string funname)
{
	return;
}
//
void WorkerThread::onGet(int conId, db_srv::get* message)
{
	if (!message) return;

	int uid = message->uid();
	std::string argback(message->argback());
	int tablesize = message->table_name_size();
	db_srv::get_reply* get_reply = new db_srv::get_reply();
	get_reply->set_uid(uid);
	get_reply->set_argback(argback);
	for (int i = 0; i < tablesize; i++)
	{
		::db_srv::get_reply_table* table = get_reply->add_tables();
		// 首先从redis 中找
		bool rres = loadFromRedis(uid, message->table_name(i), table);
		if (!rres)
		{
			// redis 没找到, 就从数据库中找
			int ret = loadFromMySql(uid, message->table_name(i), table);
			if( ret != 0)
			{
				table->set_table_name(message->table_name(i));
				table->set_table_bin("");
				table->set_table_status(ret); // 数据库中没有找到这个玩家的原因
			}
		}
	}

	// 现在可以释放掉这个消息包了
	delete message;
	sendReply(conId, get_reply);
}
////////////////////////////////////////
void WorkerThread::onSet(int conId, db_srv::set* message)
{
	if (!message) return;
	int uid = message->uid();
	std::string argback(message->argback());
	int tablesize = message->tables_size();
	db_srv::set_reply* set_reply = new db_srv::set_reply();
	set_reply->set_uid(uid);
	set_reply->set_argback(argback);
	for (int i = 0; i < tablesize; i++)
	{
		::db_srv::set_reply_table_status* table_status = set_reply->add_table_statuses();
		// 首先保存到redis 中
		if (saveToRedis(uid, message->tables(i), table_status))
		{
			// 保存redis成功后，再异步保存到mysql 中
			saveToMySql(uid, message->tables(i), table_status);
		}
	}

	// 现在可以释放掉这个消息包了
	delete message;
	sendReply(conId, set_reply);
}

bool WorkerThread::saveToRedis(int uid, const ::db_srv::set_table& set_table, ::db_srv::set_reply_table_status* status)
{
	char rediskey[100];
	snprintf(rediskey, 99, "%s:%d", set_table.table_name().c_str(), uid);
	redisReply* reply = static_cast<redisReply*>(readis_.RedisCommand("SET %s %b", rediskey, set_table.table_bin().c_str(), set_table.table_bin().length()));
	if (!reply)
	{
    	status->set_table_name(set_table.table_name());
    	status->set_status("REDISERROR");
		return false;
	}

    if (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"ok") == 0)
    {
    	status->set_table_name(set_table.table_name());
    	status->set_status("OK");
    	readis_.FreeReplyObject(reply);
    	return true;
	}

	status->set_table_name(set_table.table_name());
	if (reply->str)
	{
		status->set_status(reply->str);
	}
	else
	{
		status->set_status("ERROR");
	}
	readis_.FreeReplyObject(reply);
	return false;
}

void WorkerThread::getRedisKey(int uid, std::string& table_name, char* outKey, int outKeyLen)
{
	snprintf(outKey, outKeyLen, "%s:%d", table_name.c_str(), uid);
	return;
}

bool WorkerThread::saveToMySql(int uid, const ::db_srv::set_table& set_table, ::db_srv::set_reply_table_status* status)
{
	std::string tablename(set_table.table_name());
	std::string typeName;
	if (isParseTable(tablename, typeName))
	{
		google::protobuf::Message* message = KabuCodec::createDynamicMessage(typeName);
		if (!message)
		{
			return false;
		}

		if (!message->ParseFromString(set_table.table_bin()))
		{
			delete message;
			return false;
		}

		std::string replaceSql(PBSql::buildReplaceSql(message, tablename, mysql_));
		if (replaceSql == "")
		{
			delete message;
			return false;
		}
		delete message;

		char* sql_buf = sWriterThreadPool.NewSqlBuf(replaceSql.length() + 10);
		snprintf(sql_buf, replaceSql.length() + 1, "%s", replaceSql.c_str());

		WriterThreadParam param;
		param.Type = WRITERTHREAD_CMD;
		param.sql = sql_buf;
		getRedisKey(uid, tablename, param.key, REDISKEY_MAXLEN);
		param.length = replaceSql.length();

		dispatchWriterThread(param);

		return true;
	}

	size_t table_bin_length = set_table.table_bin().length();
	// 大于20k的时候 打个警告的日志吧
	if (table_bin_length >= 1024 * 20)
	{
		LOGEX_WARN("table bin too length, uid: %d, table_name:%s, length: %d", uid, set_table.table_name().c_str(), table_bin_length);
	}

	char str_rs[1024 * 100]; // 100k空间
	unsigned long len = mysql_.format_to_real_string(str_rs, set_table.table_bin().c_str(), table_bin_length);
	if (len == 0)
	{
		// 数据库有问题了吧
		LOGEX_WARN("format_to_real_string error, uid: %d, table_name:%s, length: %d", uid, set_table.table_name().c_str(), table_bin_length);
		return false;
	}

	char* sql_buf = sWriterThreadPool.NewSqlBuf(len + 200);
	// replace into tablename(uid, table_bin, update_time) values(%d, %s, %d)
	// 一张表的时候的sql语句
	static const char sql[] = "replace into %s(uid, table_bin, update_time) values(%d, '%s', %d)";
	// 多张表的时候的sql语句
	static const char sql2[] = "replace into %s_%d(uid, table_bin, update_time) values(%d, '%s', %d)";
	// 该类型的表 有多少张
	int tablenum = sConfigMgr.MainConfig.GetIntDefault("table", set_table.table_name().c_str(), 1);

	int sqlbuflen = 0;
	if (tablenum == 1)
	{
		sqlbuflen = snprintf(sql_buf, len + 200, sql, set_table.table_name().c_str(), uid, str_rs, time(NULL));
	}
	else
	{
		sqlbuflen = snprintf(sql_buf, len + 200, sql2, set_table.table_name().c_str(), uid % tablenum, uid, str_rs, time(NULL));
	}


	WriterThreadParam param;
	param.Type = WRITERTHREAD_CMD;
	param.sql = sql_buf;
	param.length = sqlbuflen;
	getRedisKey(uid, tablename, param.key, REDISKEY_MAXLEN);
	dispatchWriterThread(param);
	return true;
}

/////////////////////////////////
void WorkerThread::onMGet(int conId, db_srv::mget* message)
{
	if (!message) return;
	int uid = message->uid();
	std::string argback(message->argback());
	int tablesize = message->user_tables_size();
	db_srv::mget_reply* mget_reply = new db_srv::mget_reply();
	mget_reply->set_uid(uid);
	mget_reply->set_argback(argback);
	for (int i = 0; i < tablesize; i++)
	{
		::db_srv::mget_reply_user_table* table = mget_reply->add_tables();
		// 首先从redis 中找
		bool rres = loadFromRedis(uid, message->user_tables(i).table_name(), table);
		if (!rres)
		{
			// redis 没找到, 就从数据库中找
			bool mres = loadFromMySql(uid, message->user_tables(i).table_name(), table);
			if (!mres)
			{
				// 连数据库都没有找到, 没有方法了只能设空了
				table->set_uid(uid);
				table->set_table_name(message->user_tables(i).table_name());
				table->set_table_bin("");
			}
		}
	}

	// 现在可以释放掉这个消息包了
	delete message;
	sendReply(conId, mget_reply);
}

bool WorkerThread::loadFromRedis(int uid, const std::string& tablename, ::db_srv::mget_reply_user_table* table)
{
	bool result = false;
	char rediskey[100];
	snprintf(rediskey, 99, "%s:%d", tablename.c_str(), uid);
	redisReply* reply = static_cast<redisReply*>(readis_.RedisCommand("GET %s", rediskey));
	if (reply && reply->type == REDIS_REPLY_STRING)
	{
		table->set_uid(uid);
		table->set_table_name(tablename);
		table->set_table_bin(reply->str, reply->len);
		result = true;
	}

	readis_.FreeReplyObject(reply);
	return result;
}

bool WorkerThread::loadFromMySql(int uid, const std::string& tablename, ::db_srv::mget_reply_user_table* table)
{
	std::string typeName;
	if (isParseTable(tablename, typeName))
	{
		google::protobuf::Message* msg = PBSql::select(tablename, typeName, uid, mysql_);
		if (!msg)
		{
			return false;
		}

		std::string table_bin;
		if (!msg->SerializeToString(&table_bin))
		{
			delete msg;
			return false;
		}

		table->set_uid(uid);
		table->set_table_name(tablename);
		table->set_table_bin(table_bin.c_str(), table_bin.length());
		delete msg;
		return true;
	}
	return false;
}

void WorkerThread::onUnknownMessage(int conId, google::protobuf::Message* message)
{
	if (!message) return;
	// 现在可以释放掉这个消息包了
	delete message;
}

#include "lua/LuaPB.h"
#include "ProtoImporter.h"
void WorkerThread::onLuaMessage(int conId, google::protobuf::Message* message)
{
	reloadLua();

	if (!message) return;
	LOG_INFO << "onLuaMessage: " << message->GetTypeName()<< message->DebugString()<< " threadid:" << id_;

	lua_State* L = luaEngine_.getLuaState();

	lua_getglobal(L, "onLuaMessage");
	if (!lua_isfunction(L, -1))
	{
		fprintf(stderr, "WorkerThread::onLuaMessage, on lua message funname not exist\n");
		return;
	}

	lua_pushinteger(L, conId);
	LuaPB::pushMessage(L, message);

	if (lua_pcall(L, 2, 1, 0) != 0)
	{
		fprintf(stderr, "lua_pcall WorkerThread::onLuaMessage error, error msg:%s\n",
				lua_tostring(L, -1));
		lua_pop(L, 1);// 从栈中弹出出错消息
		return;
	}

	int ir = lua_gettop(L);

	if (ir > 0)
	{
		int tr = static_cast<int>(lua_tonumber(L, -1));
		lua_pop(L, ir);

		printf("WorkerThread::onLuaMessage ret:%d\n", tr);
	}

    return;
}
