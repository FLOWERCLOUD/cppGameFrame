
#ifndef GAME_TOOLSCONFIGMGR_H_
#define GAME_TOOLSCONFIGMGR_H_

#include <mysdk/base/Singleton.h>
#include <mysdk/exconfig/Config.h>

class  ToolsConfigMgr
{
public:
	ConfigFile MainConfig;
};

#define sToolsConfigMgr mysdk::Singleton<ToolsConfigMgr>::instance()

#endif
