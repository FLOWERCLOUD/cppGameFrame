/*
 * Base.h
 *
 *  Created on: 2012-4-6
 *    
 */

#ifndef GAME_BASE_H_
#define GAME_BASE_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>
#include <mysdk/base/Logging.h>

#include <mysdk/database/mysql/DatabaseWorkerPool.h>
#include <mysdk/database/mysql/DatabaseWorkerPool.cc>
#include <mysdk/database/mysql/MySQLConnectionInfo.h>
#include <mysdk/database/mysql/QueryResult.h>
#include <mysdk/database/mysql/Field.h>

#include <game/LoongBgSrv/base/ColonBuf.h>

using namespace mysdk;

typedef DatabaseWorkerPool<MySQLConnection> TestDatabaseWorkerPool;

class Base
{
public:
	Base();
	~Base();

	bool init(const std::string& host, const std::string& port_or_socket, const std::string& user, const std::string& password, const std::string& database);
private:

private:
	DISALLOW_COPY_AND_ASSIGN(Base);
};

#define sBase mysdk::Singleton<Base>::instance()

#endif /* BASE_H_ */
