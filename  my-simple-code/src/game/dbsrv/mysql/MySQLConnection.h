
#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include "MySQLConnectionInfo.h"

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysql.h>

using namespace mysdk;

class ResultSet;
class MySQLConnection
{
public:
		MySQLConnection():
			reconnectiong_(false),
			mysql_(NULL)
		{
		}
		MySQLConnection(const MySQLConnectionInfo& connInfo);
		~MySQLConnection();

		bool open();
		void close();

		void setConnectionInfo(const MySQLConnectionInfo& connInfo)
		{
			connectionInfo_ = connInfo;
		}
public:
		bool execute(const char* sql);
		bool execute(const char* sql, unsigned long length);
		ResultSet* query(const char* sql);

		unsigned long format_to_real_string(char *to, const char *from, unsigned long length);

		operator bool() const { return mysql_ != NULL; }
		void ping() { mysql_ping(mysql_); }

		uint32 getLastError() { return mysql_errno(mysql_); }

private:
		bool _query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount);
		MYSQL* getHandle() { return mysql_; }
        bool handleMySQLErrno(int errNo);
private:
        bool reconnectiong_; // Are we reconnectiong?
        MYSQL* mysql_; // MySQL Handle
        MySQLConnectionInfo connectionInfo_; // Connection info (used for loggging)
};

#endif
