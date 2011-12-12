/*
 * DatabaseWorkerThread.h
 *
 *  Created on: 2011-11-3
 *    
 */

#ifndef MYSDK_DATABASE_MYSQL_DATABASEWORKERTHREAD_H_
#define MYSDK_DATABASE_MYSQL_DATABASEWORKERTHREAD_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Thread.h>
#include <mysdk/base/YPipeT.h>
#include <mysdk/base/BlockingQueue.h>

//#include <mysdk/net/EventLoop.h>

#include <mysdk/database/mysql/DatabaseWorkerThread.h>
#include <mysdk/database/mysql/MySQLConnection.h>
#include <mysdk/database/mysql/MySQLBinds.h>
#include <mysdk/database/mysql/Transaction.h>

namespace mysdk
{
class BaseClosure
{
public:
	BaseClosure(MySQLConnection* conn, bool needDelParam) :
		conn_(conn),
		needDelParam_(needDelParam)
	{
	}

	virtual ~BaseClosure() {}

	virtual void Run() = 0;

protected:
	MySQLConnection* conn_;
	bool needDelParam_;
private:
	DISALLOW_COPY_AND_ASSIGN(BaseClosure);
};

class PingClosure : public BaseClosure
{
public:
	PingClosure(MySQLConnection* conn, bool needDelParam):
		BaseClosure(conn, needDelParam)
	{
	}

	virtual ~PingClosure() {}

	void Run()
	{
		if (conn_)
		{
			conn_->ping();
		}
	}
private:
	DISALLOW_COPY_AND_ASSIGN(PingClosure);
};

class BasicClosure : public BaseClosure
{
public:
	BasicClosure(MySQLConnection* conn , bool needDelParam, const char* sql) :
		BaseClosure(conn, needDelParam),
		sql_(sql)
	{
	}

	virtual ~BasicClosure() {};

	virtual void Run()
	{
		if (conn_)
		{
			conn_->execute(sql_);
		}
		if (needDelParam_) delete sql_;
	}

private:
	const char* sql_;
private:
	DISALLOW_COPY_AND_ASSIGN(BasicClosure);
};

class BindClosure : public BaseClosure
{
public:
	BindClosure(MySQLConnection* conn, bool needDelParam, MySQLBinds* bind):
		BaseClosure(conn, needDelParam),
		bind_(bind)
	{

	}

	virtual ~BindClosure() {}

	virtual void Run()
	{
		if (conn_)
		{
			conn_->execute(bind_);
		}
		if (needDelParam_) delete bind_;
	}
private:
	MySQLBinds* bind_;
private:
	DISALLOW_COPY_AND_ASSIGN(BindClosure);
};

class TransactionClosure : public BaseClosure
{
public:
	TransactionClosure(MySQLConnection* conn, bool needDelParam, Transaction* trans):
		BaseClosure(conn, needDelParam),
		trans_(trans)
	{

	}

	virtual ~TransactionClosure() {}

	virtual void Run()
	{
		if (conn_)
		{
			conn_->beginTransaction();
			conn_->executeTransaction(trans_);
			conn_->commitTransaction();
		}
		if (needDelParam_) delete trans_;
	}

private:
	Transaction* trans_;
private:
	DISALLOW_COPY_AND_ASSIGN(TransactionClosure);
};

//using namespace net;
class EventLoop;
class DatabaseWorkerThread
{
public:
	static const int sPipeSize = 128;

public:
	DatabaseWorkerThread(EventLoop* loop, MySQLConnection* mysqlConn);
	~DatabaseWorkerThread();

	void start();
	void join();

	void push(const char* sql);
	void push(MySQLBinds* bind);
	void push(Transaction* trans);
	bool push(BaseClosure* closure);
	void quit();
private:
	void handleClosure();
private:
	void ping();
private:
	EventLoop* loop_;
	//YPipeT<Closure*, sPipeSize> pipe_;
	BlockingQueue<BaseClosure*> pipe_;
	Thread thread_;
	MySQLConnection* mysqlConn_;
	bool quit_;
private:
	DISALLOW_COPY_AND_ASSIGN(DatabaseWorkerThread);
};

}

#endif /* DATABASEWORKERTHREAD_H_ */
