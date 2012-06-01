/*
 * ConfigMgr.h
 *
 *  Created on: 2012-4-9
 *    
 */

#ifndef GAME_CONFIGMGR_H_
#define GAME_CONFIGMGR_H_

#include <mysdk/base/Singleton.h>
#include <mysdk/exconfig/Config.h>

class  ConfigMgr
{
public:
	ConfigFile MainConfig;
};

#define sConfigMgr mysdk::Singleton<ConfigMgr>::instance()

#endif /* CONFIGMGR_H_ */
