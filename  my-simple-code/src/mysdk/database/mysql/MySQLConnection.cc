
#include <mysdk/database/mysql/MySQLConnection.h>

#include <mysdk/database/mysql/BindResultSet.h>
#include <mysdk/database/mysql/DatabaseWorkerThread.h>
#include <mysdk/database/mysql/MySQLConnection.h>
#include <mysdk/database/mysql/MySQLConnectionInfo.h>
#include <mysdk/database/mysql/MySQLBinds.h>
#include <mysdk/database/mysql/MySQLStmt.h>
#include <mysdk/database/mysql/QueryResult.h>
#include <mysdk/database/mysql/Transaction.h>

#include <mysdk/base/Logging.h>

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

using namespace mysdk;

MySQLConnection::MySQLConnection(const MySQLConnectionInfo& connInfo):
		reconnectiong_(false),
		prepareError_(false),
		mysql_(NULL),
		connectionInfo_(connInfo)
{
	//mysql_library_init(0, NULL, NULL);
}

MySQLConnection::~MySQLConnection()
{
    //assert (mysql_); /// MySQL context must be present at this point
    //mysql_close(mysql_);

	MySQLStmtMap_t::const_iterator itr;
    for (itr = stmtMap_.begin(); itr != stmtMap_.end(); ++itr)
    {
    	MySQLStmt* stmt = itr->second;
    	delete stmt;
    }
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
		LOG_ERROR << "could not initalize Mysql connection to database: "
								<< connectionInfo_.getDatabase();
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
    	LOG_ERROR << "Could not connect to MySQL database at["
    			<< connectionInfo_.getHost()
    			<< ":" << mysql_error(mysqlInit) << "]";

        mysql_close(mysqlInit);
        return false;
    }

    if (!reconnectiong_)
    {
		LOG_TRACE << "MySQL client library:  " << mysql_get_client_info();
		LOG_TRACE << "MySQL server ver: " << mysql_get_server_info(mysql_);
		if (mysql_get_server_version(mysql_) != mysql_get_client_version())
		{
			LOG_WARN << "MySQL client/server version mismatch; may conflict with behaviour of prepared statements.";
		}
    }

    LOG_TRACE << "Connected to MySQL database at: " << connectionInfo_.getDatabase();
    mysql_autocommit(mysql_, 1);

    // set connection properties to UTF8 to properly handle locales for different
    // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
    mysql_set_character_set(mysql_, "utf8");
    return interRegisterStmt();
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

MySQLStmt* MySQLConnection::getMySQLStmt(uint32 index)
{
	MySQLStmtMap_t::iterator it = stmtMap_.find(index);
	if (it == stmtMap_.end())
	{
    	LOG_ERROR << "Could not fetch prepared statement "
								<< index
								<< " on database "
								<< connectionInfo_.getDatabase();
		return NULL;
	}
	else
	{
		return it->second;
	}
}

bool MySQLConnection::execute(MySQLBinds* bind)
{
    if (!mysql_)
    {
    	return false;
    }

    uint32 index = bind->getIndex();
    {
    	MySQLStmt* stmt = getMySQLStmt(index);
        assert(stmt);            // Can only be null if preparation failed, server side error or bad query

        MYSQL_STMT* msql_STMT = stmt->getMysqlStmt();
        MYSQL_BIND* msql_BIND = bind->getMysqlBind();
        assert(msql_STMT);
        assert(msql_BIND);
        if (mysql_stmt_bind_param(msql_STMT, msql_BIND))
        {
            uint32 lErrno = mysql_errno(mysql_);
            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
            	return execute(bind);       // Try again
            }
            return false;
        }

        if (mysql_stmt_execute(msql_STMT))
        {
            uint32 lErrno = mysql_errno(mysql_);
            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
            	return execute(bind);       // Try again
            }

            return false;
        }

        return true;
    }
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

    if (!*pRowCount)
    {
        mysql_free_result(*pResult);
        return false;
    }

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

BindResultSet* MySQLConnection::query(MySQLBinds* bind)
{
    MYSQL_RES *result = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if (!_query(bind, &result, &rowCount, &fieldCount))
    {
    	return NULL;
    }

    if (mysql_more_results(mysql_))
    {
        mysql_next_result(mysql_);
    }

    uint32 index = bind->getIndex();
    MySQLStmt* stmt = getMySQLStmt(index);
    return new BindResultSet(stmt->getMysqlStmt(), result, rowCount, fieldCount);
}

bool MySQLConnection::_query(MySQLBinds* bind, MYSQL_RES **pResult, uint64* pRowCount, uint32* pFieldCount)
{
    if (!mysql_)
    {
    	return false;
    }

    uint32 index = bind->getIndex();
    {
        MySQLStmt* stmt = getMySQLStmt(index);
        assert(stmt);            // Can only be null if preparation failed, server side error or bad query

        MYSQL_STMT* msql_STMT = stmt->getMysqlStmt();
        MYSQL_BIND* msql_BIND = bind->getMysqlBind();
        assert(msql_STMT);
        assert(msql_BIND);
        if (mysql_stmt_bind_param(msql_STMT, msql_BIND))
        {
            uint32 lErrno = mysql_errno(mysql_);
            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
            	return _query(bind, pResult, pRowCount, pFieldCount);       // Try again
            }

            return false;
        }

        if (mysql_stmt_execute(msql_STMT))
        {
            uint32 lErrno = mysql_errno(mysql_);
            if (handleMySQLErrno(lErrno))  // If it returns true, an error was handled successfully (i.e. reconnection)
            {
            	return _query(bind, pResult, pRowCount, pFieldCount);      // Try again
            }

            return false;
        }

        *pResult = mysql_stmt_result_metadata(msql_STMT);
        *pRowCount = mysql_stmt_num_rows(msql_STMT);
        *pFieldCount = mysql_stmt_field_count(msql_STMT);

        return true;
    }
}

void MySQLConnection::beginTransaction()
{
    execute("START TRANSACTION");
}

void MySQLConnection::rollbackTransaction()
{
    execute("ROLLBACK");
}

void MySQLConnection::commitTransaction()
{
    execute("COMMIT");
}

bool MySQLConnection::executeTransaction(Transaction* transaction)
{
    std::list<SQLElementData> const& queries = transaction->getQueries();
    if (queries.empty())
    {
    	return false;
    }

    beginTransaction();

    std::list<SQLElementData>::const_iterator itr;
    for (itr = queries.begin(); itr != queries.end(); ++itr)
    {
        SQLElementData const& data = *itr;
        switch (itr->type)
        {
            case SQL_ELEMENT_PREPARED:
            {
                MySQLBinds* bind = data.element.bind;
                assert(bind);
                if (!execute(bind))
                {
                	LOG_WARN << "[Warning] Transaction aborted. "
											<< queries.size()
											<<" queries not executed.";
                    rollbackTransaction();
                    return false;
                }
            }
            break;
            case SQL_ELEMENT_RAW:
            {
                const char* sql = data.element.query;
                assert(sql);
                if (!execute(sql))
                {
                	LOG_WARN << "[Warning] Transaction aborted. "
											<< queries.size()
											<<" queries not executed.";
                    rollbackTransaction();
                    return false;
                }
            }
            break;
        }
    }

    // we might encounter errors during certain queries, and depending on the kind of error
    // we might want to restart the transaction. So to prevent data loss, we only clean up when it's all done.
    // This is done in calling functions DatabaseWorkerPool<T>::DirectCommitTransaction and TransactionTask::Execute,
    // and not while iterating over every element.
    commitTransaction();
    return true;
}

bool MySQLConnection::handleMySQLErrno(uint32 errNo)
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
            sleep(3);                           // Sleep 3 seconds
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

void MySQLConnection::registerStmt(uint8 index, const char* sql, ConnectionFlags connFlags)
{
	MySQLStmtMap_t::iterator it = stmtMap_.find(index);
	if (it != stmtMap_.end())
	{
    	LOG_ERROR << "have prepared statement "
								<< index
								<< " on database "
								<< connectionInfo_.getDatabase();
	}

	LOG_TRACE << " registerStmt, index " << index << " sql: " << sql;
	stmtMap_[index] = new MySQLStmt(mysql_, index, sql, connFlags);
}

bool MySQLConnection::interRegisterStmt()
{
	return true;
}
