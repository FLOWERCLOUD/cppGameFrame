/*
 * bind_test.cc
 *
 *  Created on: 2011-11-8
 *    
 */

#include <mysdk/database/mysql/tests/TestDatabase.h>
#include <mysdk/database/mysql/MySQLBinds.h>
#include <mysdk/database/mysql/BindResultSet.h>
#include <mysdk/database/mysql/QueryResult.h>
#include <mysdk/database/mysql/Field.h>

#include <mysdk/base/Logging.h>

#include <string>

using namespace mysdk;

int main()
{
	TestDatabaseWorkerPool databaseWorkerPool;
	std::string host = "192.168.100.6";
	std::string port_or_socket = "3306";
	std::string user = "root";
	std::string password = "4399mysql#CQPZM";
	std::string database = "first_db";

	MySQLConnectionInfo connInfo(host, port_or_socket, user, password, database);
	if (!databaseWorkerPool.open(NULL, connInfo, 2, 1))
		return -1;
	/*
	uint32 index = 1;
	uint8 paramCount = 1;
	MySQLBinds bind(index, paramCount);
	bind.setInt32(0, 11353);
	BindResultSet* res = databaseWorkerPool.query(&bind);
	if (res)
	{
		uint64 rowCount = res->getRowCount();
		LOG_TRACE << "have result set!!! rowCount: " << rowCount;
		//uint32 fieldCount = res->getFieldCount();
		do
		{
			const Field* field = res->fetch();
			LOG_TRACE << "--------------------------";
			LOG_TRACE << "uid: " << field[0].getInt32();
			LOG_TRACE << "username: " << field[1].getString();
			//LOG_TRACE << "--------------------------";
		}while (res->nextRow());

		delete res;
	}
	*/
	uint32 index = 3;
	uint8 paramCount = 2;
	MySQLBinds bind2(index, paramCount);
	bind2.setString(0, "test1");
	bind2.setInt32(1, 1);
	databaseWorkerPool.execute(&bind2);

	sleep(10);
	databaseWorkerPool.close();
	return 0;
}
