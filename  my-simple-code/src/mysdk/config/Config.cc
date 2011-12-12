/*
 * Config.cc
 *
 *  Created on: 2011-10-24
 *    
 */

#include <mysdk/config/Config.h>

#include <mysdk/base/Logging.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace mysdk;

Config::Config()
{
}

Config::~Config()
{
}

std::string Config::get(const std::string& key, std::string defaultValue)
{
	ConfigMap::iterator iter = m_configMap.find(key);
	if (iter != m_configMap.end())
	{
		return iter->second;
	}
	return defaultValue;
}

void Config::printfInfo()
{
	ConfigMap::iterator iter;
	for (iter = m_configMap.begin(); iter != m_configMap.end(); ++iter)
	{
		std::string key = iter->first;
		std::string value = iter->second;
		LOG_INFO << "[" << key << "]" << " ===> " << "[" << value << "]";
	}
}

bool Config::parse(const char* content, size_t conLength)
{
	assert(strlen(content) == conLength);

	size_t count = 0;
	std::string text;
	while(count < conLength)
	{
		text.clear();
		while (count < conLength)
		{
			char c = content[count++];
			if ( c == '\n' || count > conLength)
			{
				break;
			}
			text.push_back(c);
		}
		// 没有内容了
		if ( text.length() <= 0)
		{
			continue;
		}
		// 是注释行
		if (text.find("#") < text.find_first_not_of("#"))
		{
			continue;
		}

		size_t idxEquals = text.find("=");
		if (idxEquals == std::string::npos || idxEquals <= 0)
		{
			continue;
		}
		// 分割key value 值
		std::string key = text.substr(0, idxEquals);
		std::string value = text.substr(idxEquals + 1, text.length() - idxEquals);
		LOG_TRACE << "["<< text << "]" << "[" << key << "]" << "[" << value << "]";
		const std::string SPACES = "\t\r\n ";
		key.erase(0, key.find_first_not_of(SPACES));
		value.erase(0, value.find_first_not_of(SPACES));

		size_t idx = key.find_last_not_of(SPACES);
		if (idx != std::string::npos)
		{
			key.erase( idx + 1);
		}

		idx = value.find_last_not_of(SPACES);
		if (idx != std::string::npos)
		{
			value.erase(idx + 1);
		}

		m_configMap[key] = value;
	}

	return true;
}
