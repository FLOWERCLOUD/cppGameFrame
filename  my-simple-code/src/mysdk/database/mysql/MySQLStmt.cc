/*
 * MySQLStmt.cc
 *
 *  Created on: 2011-11-5
 *    
 */

#include <mysdk/database/mysql/MySQLStmt.h>

#include <mysdk/base/Logging.h>

#include <assert.h>
using namespace mysdk;

MySQLStmt::MySQLStmt(MYSQL* mysql, uint8 index, const char* sql, ConnectionFlags connFlags):
		index_(index),
		connFlags_(connFlags),
		stmt_(NULL),
		mysql_(mysql),
		sql_(sql)
{
	LOG_TRACE << "MySQLStmt index: " << index << " sql: " << sql;
	init();
}

MySQLStmt::~MySQLStmt()
{
	if (stmt_)
	{
		mysql_stmt_close(stmt_);
	}
}

bool MySQLStmt::init()
{
	assert(mysql_);
    MYSQL_STMT * stmt = mysql_stmt_init(mysql_);
    if (!stmt)
    {
    	LOG_ERROR << "In mysql_stmt_init() id: " << index_ << ", sql: " << sql_;
    	//LOG_ERROR << myql_error(mysql_);
    	return false;
    }

    if ( mysql_stmt_prepare(stmt, sql_.c_str(), static_cast<unsigned long>(sql_.length())) )
    {
        	LOG_ERROR << "In mysql_stmt_prepare() id: "<< index_ << ", sql: "  << sql_;
        	LOG_ERROR << mysql_stmt_error(stmt);
            mysql_stmt_close(stmt);
          	return false;
     }

    stmt_ = stmt;
    return true;
}
