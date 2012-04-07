/*
 * Base.cc
 *
 *  Created on: 2012-4-6
 *    
 */

#include <game/LoongBgSrv/base/Base.h>

#include <game/LoongBgSrv/base/BufBaseMgr.h>
#include <game/LoongBgSrv/base/ItemBaseMgr.h>
#include <game/LoongBgSrv/base/PetBaseMgr.h>
#include <game/LoongBgSrv/base/SkillBaseMgr.h>

Base::Base()
{

}

Base::~Base()
{

}

bool Base::init(const std::string& host, const std::string& port_or_socket, const std::string& user, const std::string& password, const std::string& database)
{
	TestDatabaseWorkerPool databaseWorkerPool;

	MySQLConnectionInfo connInfo(host, port_or_socket, user, password, database);
	if (!databaseWorkerPool.open(NULL, connInfo, 2, 1))
		return false;

	LOG_INFO << " START init data!!!";
	if(!sBufBaseMgr.init(databaseWorkerPool))
	{
		LOG_ERROR << "init buff error!!\n";
		databaseWorkerPool.close();
		return false;
	}
	if (!sItemBaseMgr.init(databaseWorkerPool))
	{
		LOG_ERROR << "init item error!!\n";
		databaseWorkerPool.close();
		return false;
	}
	if (!sPetBaseMgr.init(databaseWorkerPool))
	{
		LOG_ERROR << "init pet error!!\n";
		databaseWorkerPool.close();
		return false;
	}
	if (!sSkillBaseMgr.init(databaseWorkerPool))
	{
		LOG_ERROR << "init skill error!!\n";
		databaseWorkerPool.close();
		return false;
	}
	LOG_INFO << " END init data!!!";
	databaseWorkerPool.close();
	return true;
}
