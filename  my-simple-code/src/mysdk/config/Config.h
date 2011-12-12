/*
 * Config.h
 *
 *  Created on: 2011-10-24
 *    
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <mysdk/base/Common.h>

#include <map>
#include <string>

class Config
{
public:
	typedef std::map<std::string, std::string> ConfigMap;

public:
	Config();
	~Config();

	std::string get(const std::string& key, std::string defaultValue = "");
	void printfInfo();
public:
	bool parse(const char* content, size_t conLength);

private:
	ConfigMap m_configMap;
private:
	DISALLOW_COPY_AND_ASSIGN(Config);
};

#endif /* CONFIG_H_ */
