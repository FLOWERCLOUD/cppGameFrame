
#ifndef MYSDK_BASE_MYSQLCONNECTION_H
#define MYSDK_BASE_MYSQLCONNECTION_H

#include <mysdk/base/Common.h>

#include <mysdk/database/mysql/MySQLConnectionInfo.h>

#include <mysql.h>

#include <string>
#include <map>
#include <vector>

namespace mysdk
{

class MySQLStmt;
class MySQLBinds;
class ResultSet;
class BindResultSet;
class Transaction;

struct PreparedStatementTable
{
    uint32 index;
    const char* query;
    ConnectionFlags type;
};

class MySQLConnection
{
public:
	typedef std::map<uint32, MySQLStmt* > MySQLStmtMap_t;

public:
	MySQLConnection(const MySQLConnectionInfo& connInfo);
	virtual ~MySQLConnection();

	virtual bool open();
	void close();

	void registerStmt(uint8 index, const char* sql, ConnectionFlags connFlags);
public:
	bool execute(const char* sql);
	bool execute(MySQLBinds* bind);
	ResultSet* query(const char* sql);
	BindResultSet* query(MySQLBinds* bind);

	bool _query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount);
	bool _query(MySQLBinds* bind, MYSQL_RES **pResult, uint64* pRowCount, uint32* pFieldCount);

	void beginTransaction();
	void rollbackTransaction();
	void commitTransaction();
	bool executeTransaction(Transaction* transaction);

	operator bool() const { return mysql_ != NULL; }
	void ping() { mysql_ping(mysql_); }

	uint32 getLastError() { return mysql_errno(mysql_); }
protected:
	MYSQL* getHandle() { return mysql_; }
	MySQLStmt* getMySQLStmt(uint32 index);
protected:
		MySQLStmtMap_t stmtMap_; // MySQL stmt storage
        bool reconnectiong_; // Are we reconnectiong?
        bool prepareError_; // Was there any error while preparing statements?

private:
        bool handleMySQLErrno(uint32 errNo);
    	virtual bool interRegisterStmt();
private:
	MYSQL* mysql_; // MySQL Handle
	MySQLConnectionInfo connectionInfo_; // Connection info (used for loggging)
	ConnectionFlags connectionFlags_; // Connection flags (for preparing relevant statements)

private:
	DISALLOW_COPY_AND_ASSIGN(MySQLConnection);
};

}

#endif
