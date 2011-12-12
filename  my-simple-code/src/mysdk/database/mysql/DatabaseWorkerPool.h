
#ifndef MYSDK_DATABASE_MYSQL_DATABASEWORKERPOOL_H
#define MYSDK_DATABASE_MYSQL_DATABASEWORKERPOOL_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Mutex.h>

#include <mysdk/net/EventLoop.h>

#include <vector>

namespace mysdk
{
//using namespace net;

class EventLoop;
class MySQLConnectionInfo;
class MySQLConnection;
class MySQLBinds;
class DBWorkerThread;
class Transaction;
class ResultSet;
class BindResultSet;
template <typename T>
class DatabaseWorkerPool
{
public:
	DatabaseWorkerPool();
	~DatabaseWorkerPool();

	bool open(EventLoop* loop, const MySQLConnectionInfo& connInfo, uint8 sync_conNums, uint8 async_threads);
	void close();

	void execute(const char* sql);
	void pExecute(const char* sql, ...);

	void execute(MySQLBinds* bind);

    void directExecute(const char* sql);
    void directPExecute(const char* sql, ...);

    ResultSet* query(const char* sql);
    ResultSet* pQuery(const char* sql, ...);

    BindResultSet* query(MySQLBinds* bind);

    /**
        Transaction context methods.
    */
    Transaction* beginTransaction();
    void endTransaction(Transaction* trans);

    void commitTransaction(Transaction* trans);
    void directCommitTransaction(Transaction* trans);

    void executeOrAppend(Transaction* trans, MySQLBinds* bind);
    void executeOrAppend(Transaction* trans, const char* sql);

private:
    DBWorkerThread* getNextWorkThread();
	T* getNextMySQLConnection();
private:
	std::vector<T* >  syncConns_;
	std::vector<T* >  asyncConns_;
	std::vector<DBWorkerThread* > asyncThreads_;
	MutexLock asyncMutex_;
	MutexLock syncMutex_;
	uint32 nextAsyncThread_;
	uint32 nextSyncConn_;
private:
	DISALLOW_COPY_AND_ASSIGN(DatabaseWorkerPool);
};

}

#endif
