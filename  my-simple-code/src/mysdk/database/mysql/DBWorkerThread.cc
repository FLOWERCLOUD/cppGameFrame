/*
 * DBWorkerThread.cc
 *
 *  Created on: 2011-11-9
 *    
 */

#include <mysdk/database/mysql/DBWorkerThread.h>
#include <mysdk/database/mysql/MySQLBinds.h>
#include <mysdk/database/mysql/Transaction.h>

#include <tr1/functional>

using namespace mysdk;
using namespace mysdk::net;

DBWorkerThread::DBWorkerThread(mysdk::net::EventLoop* loop, MySQLConnection* mysqlConn):
		loop_(loop),
		thread_(std::tr1::bind(&DBWorkerThread::run, this), "DBWorkerThread"),
		mysqlConn_(mysqlConn),
		quit_(false)
{
}

DBWorkerThread::~DBWorkerThread()
{

}

void DBWorkerThread::start()
{
	thread_.start();
}

void DBWorkerThread::join()
{
	thread_.join();
}

void DBWorkerThread::quit()
{
	quit_ = true;
}

void DBWorkerThread::run()
{
	LOG_INFO << "START DBWorkerThread...";
	while(!quit_)
	{
		//LOG_TRACE << "========RUN ... ";
		DBMsg msg;
		if (pipe_.read(&msg) )
		{
			handleDBMsg(&msg);
		}
		sleep(1);
	}
	LOG_INFO << "END DBWorkerThread...";
}

void DBWorkerThread::handleDBMsg(DBMsg* msg)
{
	LOG_TRACE << "===== handleDBMsg ... ";
	assert(msg);
	DBMsgType type = msg->type;
	if (type == SQL_DBTYPE)
	{
		mysqlConn_->execute(msg->param.sql);
	}
	else if (type == BIND_DBTYPE)
	{
		mysqlConn_->execute(msg->param.bind);
	}
	else if (type == TRANS_DBTYPE)
	{
		mysqlConn_->beginTransaction();
		mysqlConn_->executeTransaction(msg->param.trans);
		mysqlConn_->commitTransaction();
	}
}

void DBWorkerThread::ping()
{

}

void DBWorkerThread::push(const DBMsg& msg)
{
	pipe_.write(msg, false);
	pipe_.flush();
}

void DBWorkerThread::push(char* sql)
{
	DBMsg msg;
	msg.type = SQL_DBTYPE;
	msg.param.sql = sql;
	push(msg);
}

void DBWorkerThread::push(MySQLBinds* bind)
{
	DBMsg msg;
	msg.type = BIND_DBTYPE;
	msg.param.bind = bind;
	push(msg);
}

void DBWorkerThread::push(Transaction* trans)
{
	DBMsg msg;
	msg.type = TRANS_DBTYPE;
	msg.param.trans = trans;
	push(msg);
}
