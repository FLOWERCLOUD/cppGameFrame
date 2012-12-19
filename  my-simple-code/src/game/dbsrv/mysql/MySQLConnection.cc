
#include "MySQLConnection.h"

#include "QueryResult.h"

#include <mysdk/base/Logging.h>

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

MySQLConnection::MySQLConnection(const MySQLConnectionInfo& connInfo):
		reconnectiong_(false),
		mysql_(NULL),
		connectionInfo_(connInfo)
{
}

MySQLConnection::~MySQLConnection()
{
}

void MySQLConnection::close()
{
    /// Only close us if we're not operating
    //delete this;
	if (mysql_)
	{
		mysql_close(mysql_);
		//mysql_library_end();
	}
}

bool MySQLConnection::open()
{
	MYSQL* mysqlInit = mysql_init(NULL);
	if (!mysqlInit)
	{
		fprintf(stderr, "could not initalize Mysql connection to database: %s\n", connectionInfo_.getDatabase().c_str());
		return false;
	}

	int port;
	char const* unix_socket;
	mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");
	if (connectionInfo_.getHost() == ".") // socket use option (Unix/Linux)
	{
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, reinterpret_cast<char const*>(&opt));
        connectionInfo_.setHost(std::string("localhost"));
        port = 0;
        unix_socket = connectionInfo_.getPortOrSocket().c_str();
	}
    else                                                    // generic case
    {
        port = atoi(connectionInfo_.getPortOrSocket().c_str());
        unix_socket = 0;
    }

    mysql_ = mysql_real_connect(mysqlInit,
																connectionInfo_.getHost().c_str(),
																connectionInfo_.getUser().c_str(),
																connectionInfo_.getPassword().c_str(),
																connectionInfo_.getDatabase().c_str(),
																port,
																unix_socket,
																0);

    if (!mysql_)
    {
    	fprintf(stderr, "Could not connect to MySQL database at[%s:%s]\n", connectionInfo_.getHost().c_str(), mysql_error(mysqlInit));
        mysql_close(mysqlInit);
        return false;
    }

    //if (!reconnectiong_)
    //{
	//	if (mysql_get_server_version(mysql_) != mysql_get_client_version())
	//	{
		//	fprintf(stderr, "MySQL client/server version mismatch; may conflict with behaviour of prepared statements.\n");
	//	}
  //  }

    //fprintf(stderr,  "Connected to MySQL database at: [%s]\n", connectionInfo_.getDatabase().c_str());
    LOG_INFO << "[MySQL] " << "Connected to MySQL database at: ["
						<< connectionInfo_.getHost().c_str()  << ":" << connectionInfo_.getDatabase() << "]";
    mysql_autocommit(mysql_, 1);

    // set connection properties to UTF8 to properly handle locales for different
    // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
    mysql_set_character_set(mysql_, "utf8");
    return true;
}

bool MySQLConnection::execute(const char* sql)
{
    if (!mysql_)
    {
    	return false;
    }

    {
        if (mysql_query(mysql_, sql))
        {
            uint32 lErrno = mysql_errno(mysql_);

            LOG_INFO << "[SQL] " << sql;
            LOG_ERROR << "[ " << lErrno << " ] " << mysql_error(mysql_);

            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
                return execute(sql);       // Try again
            }
            return false;
        }
    }

    return true;
}

bool MySQLConnection::execute(const char* sql, unsigned long length)
{
    if (!mysql_)
    {
    	return false;
    }

    {
        if (mysql_real_query(mysql_, sql, length))
        {
            uint32 lErrno = mysql_errno(mysql_);

            LOG_INFO << "[SQL] " << sql;
            LOG_ERROR << "[ " << lErrno << " ] " << mysql_error(mysql_);

            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
                return execute(sql, length);       // Try again
            }
            return false;
        }
    }

    return true;
}

bool MySQLConnection::query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount)
{
	if (!sql)
	{
		return false;
	}
	return _query(sql, pResult, pFields, pRowCount, pFieldCount);
}

bool MySQLConnection::_query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount)
{
    if (!mysql_)
    {
    	return false;
    }

    {
        if (mysql_query(mysql_, sql))
        {
            uint32 lErrno = mysql_errno(mysql_);
            if (handleMySQLErrno(lErrno))      // If it returns true, an error was handled successfully (i.e. reconnection)
            {
            	return _query(sql, pResult, pFields, pRowCount, pFieldCount);    // We try again
            }

            return false;
        }

        *pResult = mysql_store_result(mysql_);
        *pRowCount = mysql_affected_rows(mysql_);
        *pFieldCount = mysql_field_count(mysql_);
    }

    if (!*pResult )
    {
    	return false;
    }

    //if (!*pRowCount)
    //{
   //     mysql_free_result(*pResult);
    //    return false;
   // }

    *pFields = mysql_fetch_fields(*pResult);
    return true;
}

ResultSet* MySQLConnection::query(const char* sql)
{
	if (!sql)
	{
		return NULL;
	}

    MYSQL_RES *result = NULL;
    MYSQL_FIELD *fields = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if (!_query(sql, &result, &fields, &rowCount, &fieldCount))
    {
    	return NULL;
    }

    return new ResultSet(result, fields, rowCount, fieldCount);
}

unsigned long MySQLConnection::format_to_real_string(char *to, const char *from, unsigned long length)
{
	if (mysql_)
	{
		return mysql_real_escape_string(mysql_, to, from, length);
	}

	return 0;
}

bool MySQLConnection::handleMySQLErrno(int errNo)
{
    switch (errNo)
    {
        case 2006:  // "MySQL server has gone away"
        case 2013:  // "Lost connection to MySQL server during query"
        case 2048:  // "Invalid connection handle"
        case 2055:  // "Lost connection to MySQL server at '%s', system error: %d"
        {
        	reconnectiong_ = true;
            uint64 oldThreadId = mysql_thread_id(getHandle());
            mysql_close(getHandle());
            if (this->open())                           // Don't remove 'this' pointer unless you want to skip loading all prepared statements....
            {
                LOG_INFO << "Connection to the MySQL server is active.";
                if (oldThreadId != mysql_thread_id(getHandle()))
                {
                	LOG_INFO << "Successfully reconnected to "
										<< connectionInfo_.getDatabase() << " @"
										<< connectionInfo_.getHost() << ":"
										<< connectionInfo_.getPortOrSocket();
                }

                reconnectiong_ = false;
                return true;
            }

            uint32 lErrno = mysql_errno(getHandle());   // It's possible this attempted reconnect throws 2006 at us. To prevent crazy recursive calls, sleep here.
            sleep(1);                           // Sleep 1 seconds
            LOG_INFO << "I[MYSQL] am sleeping!!!";
            return handleMySQLErrno(lErrno);           // Call self (recursive)
        }

        case 1213:      // "Deadlock found when trying to get lock; try restarting transaction"
            return false;    // Implemented in TransactionTask::Execute and DatabaseWorkerPool<T>::DirectCommitTransaction
        // Query related errors - skip query
        case 1058:      // "Column count doesn't match value count"
        case 1062:      // "Duplicate entry '%s' for key '%d'"
        case 1054:      // "Unknown column '%s' in 'order clause'"
            return false;

        default:
        	LOG_INFO << "Unhandled MySQL errno "
								<< errNo << ". Unexpected behaviour possible.";
            return false;
    }
}
