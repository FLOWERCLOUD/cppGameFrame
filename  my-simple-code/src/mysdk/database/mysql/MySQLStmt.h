/*
 * MySQLStmt.h
 *
 *  Created on: 2011-11-5
 *    
 */

#ifndef MYSDK_DATABASE_MYSQL_MYSQLSTMT_H_
#define MYSDK_DATABASE_MYSQL_MYSQLSTMT_H_

#include <mysdk/base/Common.h>
#include <mysdk/database/mysql/MySQLConnectionInfo.h>

#include <mysql.h>
#include <string>

namespace mysdk
{

class MySQLStmt
{
public:
	MySQLStmt(MYSQL* mysql, uint8 index, const char* sql, ConnectionFlags connFlags);
	~MySQLStmt();

	MYSQL_STMT* getMysqlStmt() { return stmt_; }
	const std::string& getSql() const { return sql_; }

private:
	bool init();
private:
	uint8 index_;
	ConnectionFlags connFlags_;
	MYSQL_STMT* stmt_;
	MYSQL* mysql_;
	std::string sql_;
private:
	DISALLOW_COPY_AND_ASSIGN(MySQLStmt);
};
}

#endif /* MYSQLSTMT_H_ */
