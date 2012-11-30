

#ifndef GAME_WORKERTHREAD_H_
#define GAME_WORKERTHREAD_H_

#include <mysdk/base/Thread.h>
#include <mysdk/base/BlockingQueue.h>

#include <game/dbsrv/cache/rcache.h>
#include <game/dbsrv/mysql/MySQLConnection.h>
#include <game/dbsrv/dispatcher.h>

#include "msg.pb.h"

using namespace mysdk;

typedef enum tagThreadMsgType
{
	PING = 1,
	CMD = 2,
	STOP = 3
} ThreadMsgType;

struct ThreadParam
{
	ThreadMsgType Type;
	int conId;
	void* msg;
};


class DBSrv;
class WorkerThread
{
public:
	WorkerThread(DBSrv* srv, int id);
	~WorkerThread();

public:
	void start();
	void stop();

	void push(struct ThreadParam& param);
private:
	void threadHandler();

	void handler(struct ThreadParam& param);

	void onGet(int conId, db_srv::get* message);
	void onSet(int conId, db_srv::set* message);
	void onMGet(int conId, db_srv::mget* message);
	void onUnknownMessage(int conId, google::protobuf::Message* message);

private:
	bool loadFromRedis(int uid, const std::string& tablename, ::db_srv::get_reply_table* table);
	bool loadFromMySql(int uid, const std::string& tablename, ::db_srv::get_reply_table* table);

	bool saveToRedis(int uid, const ::db_srv::set_table& set_table, ::db_srv::set_reply_table_status* status);
	bool saveToMySql(int uid, const ::db_srv::set_table& set_table, ::db_srv::set_reply_table_status* status);

	bool loadFromRedis(int uid, const std::string& tablename, ::db_srv::mget_reply_user_table* table);
	bool loadFromMySql(int uid, const std::string& tablename, ::db_srv::mget_reply_user_table* table);
private:
	void sendReply(int conId, google::protobuf::Message* message);
private:
	int id_;
	BlockingQueue<ThreadParam> queue_;
	Thread thread_;
	DBSrv* srv_;
	rcache::Cache readis_;
	MySQLConnection mysql_;
	ProtobufDispatcher dispatcher_;
};

#endif /* WORKER_H_ */
