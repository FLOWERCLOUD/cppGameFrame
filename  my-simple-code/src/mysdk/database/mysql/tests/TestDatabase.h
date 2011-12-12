/*
 * TestDatabase.h
 *
 *  Created on: 2011-11-9
 *    
 */

#ifndef TESTDATABASE_H_
#define TESTDATABASE_H_

#include <mysdk/database/mysql/MySQLConnection.h>
#include <mysdk/database/mysql/DatabaseWorkerPool.h>
#include <mysdk/database/mysql/DatabaseWorkerPool.cc>

using namespace mysdk;
class TestDatabase:public MySQLConnection
{
public:
	TestDatabase(const MySQLConnectionInfo& connInfo);
	~TestDatabase();

private:
	virtual bool interRegisterStmt();
private:
	DISALLOW_COPY_AND_ASSIGN(TestDatabase);
};

typedef DatabaseWorkerPool<TestDatabase> TestDatabaseWorkerPool;

#endif /* TESTDATABASE_H_ */
