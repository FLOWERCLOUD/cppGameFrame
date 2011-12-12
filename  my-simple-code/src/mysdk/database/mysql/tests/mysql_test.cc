/*
 * mysql_test.cc
 *
 *  Created on: 2011-11-7
 *    
 */


#include <mysdk/database/mysql/DatabaseWorkerPool.h>
#include <mysdk/database/mysql/DatabaseWorkerPool.cc>
#include <mysdk/database/mysql/MySQLConnectionInfo.h>
#include <mysdk/database/mysql/QueryResult.h>
#include <mysdk/database/mysql/Field.h>

#include <mysdk/base/Logging.h>

#include <string>

using namespace mysdk;

typedef DatabaseWorkerPool<MySQLConnection> TestDatabaseWorkerPool;
int main()
{
	TestDatabaseWorkerPool databaseWorkerPool;
	std::string host = "192.168.100.6";
	std::string port_or_socket = "3306";
	std::string user = "root";
	std::string password = "4399mysql#CQPZM";
	std::string database = "green_journey";

	MySQLConnectionInfo connInfo(host, port_or_socket, user, password, database);
	if (!databaseWorkerPool.open(NULL, connInfo, 2, 1))
		return -1;

	const char* sql = "select uid, username from user  limit 10";
	ResultSet* res = databaseWorkerPool.query(sql);
	if (res)
	{
		//uint32 fieldCount = res->getFieldCount();
		while (res->nextRow())
		{
			const Field* field = res->fetch();
			LOG_TRACE << "--------------------------";
			LOG_TRACE << "uid: " << field[0].getInt32();
			LOG_TRACE << "username: " << field[1].getString();
			//LOG_TRACE << "--------------------------";
		}
		delete res;
	}
	databaseWorkerPool.close();
	return 0;
}
