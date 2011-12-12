/*
 * DatabaseWorkerThread.cpp
 *
 *  Created on: 2011-11-3
 *    
 */

#include <mysdk/database/mysql/DatabaseWorkerThread.h>

#include <mysdk/base/Logging.h>
#include <mysdk/net/EventLoop.h>

#include <tr1/functional>

using namespace mysdk;
using namespace internal;
using namespace net;
DatabaseWorkerThread::DatabaseWorkerThread(EventLoop* loop, MySQLConnection* mysqlConn):
		loop_(loop),
		thread_(std::tr1::bind(&DatabaseWorkerThread::handleClosure, this), "DatabaseWorkerThread"),
		mysqlConn_(mysqlConn),
		quit_(false)
{
	LOG_TRACE << "DatabaseWorkerThread";
	if (loop_)
	{
		//loop_->runEvery(10, std::tr1::bind(&DatabaseWorkerThread::ping, this));
	}
}

DatabaseWorkerThread::~DatabaseWorkerThread()
{
	LOG_TRACE << "~DatabaseWorkerThread";
}

void DatabaseWorkerThread::start()
{
	thread_.start();
}

void DatabaseWorkerThread::join()
{
	LOG_TRACE << "DatabaseWorkerThread::join";
	thread_.join();
	LOG_TRACE << "DatabaseWorkerThread::join";
}

void DatabaseWorkerThread::push(const char* sql)
{
	BaseClosure* basic = new BasicClosure(mysqlConn_, false, sql);
	push(basic);
}

void DatabaseWorkerThread::push(MySQLBinds* bind)
{
	BaseClosure* bindClosure = new BindClosure(mysqlConn_, false, bind);
	push(bindClosure);
}

void DatabaseWorkerThread::push(Transaction* trans)
{
	BaseClosure* transClosure = new TransactionClosure(mysqlConn_, false, trans);
	push(transClosure);
}

bool DatabaseWorkerThread::push(BaseClosure* closure)
{
	//pipe_.write(closure, true);
	pipe_.put(closure);
	return true;
}

void DatabaseWorkerThread::quit()
{
	quit_ = true;
}

void DatabaseWorkerThread::handleClosure()
{
	LOG_TRACE << "==== START handleClosure";
	while(!quit_)
	{	LOG_TRACE << "***************** handleClosure";
		if (pipe_.size() > 0 )
		{
			BaseClosure* closure = pipe_.take();
			if (closure)
			{
				LOG_TRACE << "++++++++++++++ handleClosure";
				closure->Run();
			}
			delete closure;
		}
		sleep(1);
	/*
		Closure** closure = NULL;
		if (pipe_.read(closure))
		{
			LOG_TRACE << "++++++++++++++ handleClosure";
			//closure->Run();
		}
		sleep(1);
		*/
	}
	LOG_TRACE << "==== END handleClosure";
}

void DatabaseWorkerThread::ping()
{
}
