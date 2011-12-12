/*
 * DatabaseWorkerPool.cc
 *
 *  Created on: 2011-11-7
 *    
 */

#include <mysdk/database/mysql/DatabaseWorkerPool.h>

#include <mysdk/database/mysql/BindResultSet.h>
#include <mysdk/database/mysql/DBWorkerThread.h>
#include <mysdk/database/mysql/MySQLConnection.h>
#include <mysdk/database/mysql/MySQLConnectionInfo.h>
#include <mysdk/database/mysql/QueryResult.h>
#include <mysdk/database/mysql/Transaction.h>

#include <mysdk/net/EventLoop.h>

#include <stdarg.h>
using namespace mysdk;

#define MAX_QUERY_LEN 32*1024

template <typename T>
DatabaseWorkerPool<T>::DatabaseWorkerPool():
		nextAsyncThread_(0),
		nextSyncConn_(0)
{
	mysql_library_init(0, NULL, NULL);

    if (!mysql_thread_safe())
    {
    	LOG_ERROR << "Used MySQL library isn't thread-safe.";
    }
    mysql_thread_init();
}

template <typename T>
DatabaseWorkerPool<T>::~DatabaseWorkerPool()
{
	mysql_thread_end();
	mysql_library_end();
}

template <typename T>
bool DatabaseWorkerPool<T>::open(EventLoop* loop, const MySQLConnectionInfo& connInfo, uint8 sync_conNums, uint8 async_threads)
{
	syncConns_.resize(sync_conNums);
	asyncConns_.resize(async_threads);
	asyncThreads_.resize(async_threads);

	for (int i = 0; i < sync_conNums; i++)
	{
		syncConns_[i] = new T(connInfo); //MySQLConnection(connInfo);
		syncConns_[i]->open();
	}

	for (int j = 0; j < async_threads; j++)
	{
		T* conn = new T(connInfo); // MySQLConnection(connInfo);
		asyncConns_[j] = conn;
		conn->open();

		asyncThreads_[j] = new DBWorkerThread(loop, conn);
	}

	for (int k = 0; k < async_threads; k++)
	{
		asyncThreads_[k]->start();
	}
	return true;
}

template <typename T>
void DatabaseWorkerPool<T>::close()
{
	int syncConnNum = syncConns_.size();
	for (int i = 0; i < syncConnNum; i++)
	{
		syncConns_[i]->close();
		delete syncConns_[i];
	}

	int async_threadsNum = asyncThreads_.size();
	for (int j = 0; j < async_threadsNum; j++)
	{
		asyncThreads_[j]->quit();
		asyncThreads_[j]->join();
	}

	for (int k = 0; k < async_threadsNum; k++)
	{
		delete asyncThreads_[k];
	}

	int asynConnNum = asyncConns_.size();
	for (int g = 0; g < asynConnNum; g++)
	{
		asyncConns_[g]->close();
		delete asyncConns_[g];
	}
}

template <typename T>
void DatabaseWorkerPool<T>::execute(const char* sql)
{
	if (!sql) return;

	DBWorkerThread* thread = getNextWorkThread();
	if (thread)
	{
		thread->push(sql);
	}
}

template <typename T>
void DatabaseWorkerPool<T>::pExecute(const char* sql, ...)
{
    if (!sql) return;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, sql);
    vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
    va_end(ap);

    execute(szQuery);
}

template <typename T>
void DatabaseWorkerPool<T>::execute(MySQLBinds* bind)
{
	//DatabaseWorkerThread* thread = getNextWorkThread();
	DBWorkerThread* thread = getNextWorkThread();

	if (thread)
	{
		thread->push(bind);
	}
}

template <typename T>
void DatabaseWorkerPool<T>::directExecute(const char* sql)
{
	if (!sql) return;

	MySQLConnection* conn = getNextMySQLConnection();
	if (conn)
	{
		conn->execute(sql);
	}
}

template <typename T>
void DatabaseWorkerPool<T>::directPExecute(const char* sql, ...)
{
    if (!sql) return;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, sql);
    vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
    va_end(ap);

    directExecute(szQuery);
}

template <typename T>
ResultSet* DatabaseWorkerPool<T>::query(const char* sql)
{
	ResultSet* resultSet = NULL;
	if (!sql) return resultSet;


	MySQLConnection* conn = getNextMySQLConnection();
	if (conn)
	{
		resultSet = conn->query(sql);
	}
	return resultSet;
}

template <typename T>
ResultSet* DatabaseWorkerPool<T>::pQuery(const char* sql, ...)
{
	if (!sql) return NULL;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, sql);
    vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
    va_end(ap);

    return query(szQuery);
}

template <typename T>
BindResultSet* DatabaseWorkerPool<T>::query(MySQLBinds* bind)
{
	BindResultSet* resultSet = NULL;

	MySQLConnection* conn = getNextMySQLConnection();
	if (conn)
	{
		resultSet = conn->query(bind);
	}
	return resultSet;
}

template <typename T>
Transaction* DatabaseWorkerPool<T>::beginTransaction()
{
	return new Transaction();
}

template <typename T>
void DatabaseWorkerPool<T>::endTransaction(Transaction* trans)
{
	delete trans;
}

template <typename T>
void DatabaseWorkerPool<T>::commitTransaction(Transaction* trans)
{
	//DatabaseWorkerThread* thread = getNextWorkThread();
	DBWorkerThread* thread = getNextWorkThread();

	if (thread)
	{
		thread->push(trans);
	}
}

template <typename T>
void DatabaseWorkerPool<T>::directCommitTransaction(Transaction* trans)
{
	MySQLConnection* conn = getNextMySQLConnection();

	if (conn)
	{
		conn->beginTransaction();
		conn->executeTransaction(trans);
		conn->commitTransaction();
	}
}

template <typename T>
void DatabaseWorkerPool<T>::executeOrAppend(Transaction* trans, MySQLBinds* bind)
{
	trans->append(bind);
}

template <typename T>
void DatabaseWorkerPool<T>::executeOrAppend(Transaction* trans, const char* sql)
{
	trans->append(sql);
}
//////////////////
template <typename T>
DBWorkerThread* DatabaseWorkerPool<T>::getNextWorkThread()
{
	DBWorkerThread* thread = NULL;

	asyncMutex_.lock();
	uint32 nextAsyncThread = nextAsyncThread_++ % asyncThreads_.size();
	thread = asyncThreads_[nextAsyncThread];
	nextAsyncThread_ = nextAsyncThread;
	asyncMutex_.unlock();

	return thread;
}

template <typename T>
T* DatabaseWorkerPool<T>::getNextMySQLConnection()
{
	T* conn = NULL;

	syncMutex_.lock();
	int nextSync = nextSyncConn_++ % syncConns_.size();
	conn = syncConns_[nextSync];
	nextSyncConn_ = nextSync;
	syncMutex_.unlock();

	return conn;
}
