/*
 * TestDatabase.cc
 *
 *  Created on: 2011-11-9
 *    
 */

#include <mysdk/database/mysql/tests/TestDatabase.h>
#include <mysdk/base/Logging.h>

TestDatabase::TestDatabase(const MySQLConnectionInfo& connInfo):
	MySQLConnection(connInfo)
{
	LOG_TRACE << "TestDatabase::TestDatabase";
}

TestDatabase::~TestDatabase()
{

}

bool TestDatabase::interRegisterStmt()
{
	//this->registerStmt(1, "select uid, username, bossrecord from user where uid = ?", CONNECTION_ASYNC);
	//this->registerStmt(2, "select uid, username from user limit ?", CONNECTION_ASYNC);
	this->registerStmt(3, "insert into testData (name, intValue) values( ?, ?)", CONNECTION_ASYNC);
	return true;
}
