/*
 * DBWorkerThread.h
 *
 *  Created on: 2011-11-9
 *    
 */

#ifndef MYSDK_DATABASE_MYSQL_DBWORKERTHREAD_H_
#define MYSDK_DATABASE_MYSQL_DBWORKERTHREAD_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Thread.h>
#include <mysdk/base/YPipeT.h>
#include <mysdk/net/EventLoop.h>

#include <mysdk/database/mysql/MySQLConnection.h>

#include <assert.h>

namespace mysdk
{
class EventLoop;
class MySQLBinds;
class MySQLConnection;
class Transaction;
typedef enum DBMsgType
{
	NONE_DBTYPE		= 0,
	SQL_DBTYPE			=	1,
	BIND_DBTYPE		=	2,
	TRANS_DBTYPE	=	3,
} DBMsgType;

typedef struct DBMsg
{
	DBMsgType type;
	union
	{
		char* sql;
		MySQLBinds* bind;
		Transaction* trans;
	} param;
	bool needDelParam;

	static DBMsg* allocateDBMsg()
	{
		return new DBMsg();
	}

	static void deallocateMsgParam(DBMsg* msg)
	{
    	DBMsgType msgtype = msg->type;
    	switch(msgtype)
    	{
    	case NONE_DBTYPE:
    		break;
    	case SQL_DBTYPE:
    		break;
    	case BIND_DBTYPE:
    		break;
    	case TRANS_DBTYPE:
    		break;
    	}

	}

   static  void deallocateDBMsg (DBMsg* msg)
    {
    	assert(msg);
    	delete msg;
    }

    DBMsg():
    	type(NONE_DBTYPE),
    	needDelParam(false)
    {
    }

    ~DBMsg()
    {
    	/*
    	if (needDelParam)
    	{
        	DBMsgType msgtype = msg->type;
        	switch(msgtype)
        	{
        	case NONE_DBTYPE:
        		break;
        	case SQL_DBTYPE:
        		delete param.sql;
        		break;
        	case BIND_DBTYPE:
        		delete param.bind;
        		break;
        	case TRANS_DBTYPE:
        		delete param.trans;
        		break;
        	}
    	}
    	*/
    }
} DBMsg;

class DBWorkerThread
{
public:
	static const int sPipeSize = 128;

public:
	DBWorkerThread(EventLoop* loop, MySQLConnection* mysqlConn);
	~DBWorkerThread();

	void start();
	void join();

	void push(char* sql);
	void push(MySQLBinds* bind);
	void push(Transaction* trans);
	void push(const DBMsg& msg);
	void quit();
private:
	void run();
	void handleDBMsg(DBMsg* msg);
private:
	void ping();
private:
	EventLoop* loop_;
	YPipeT<DBMsg, sPipeSize> pipe_;
	Thread thread_;
	MySQLConnection* mysqlConn_;
	bool quit_;
private:
	DISALLOW_COPY_AND_ASSIGN(DBWorkerThread);
};

}

#endif /* DBWORKERTHREAD_H_ */
