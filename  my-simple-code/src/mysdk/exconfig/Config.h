
#ifndef MYSDK_EXCONFIG_CONFIG_H_
#define MYSDK_EXCONFIG_CONFIG_H_

#include <mysdk/base/Common.h>

#include <string>
#include <map>

using namespace mysdk;
using namespace std;

#ifndef EOL
#ifdef WIN32
#define EOL "\r\n"
#else
#define EOL "\n"
#endif
#endif

#ifndef EOL_SIZE
#ifdef WIN32
#define EOL_SIZE 2
#else
#define EOL_SIZE 1
#endif
#endif

struct ConfigSetting
{
	string AsString;
	bool AsBool;
	int AsInt;
	float AsFloat;
};

typedef map<uint32, ConfigSetting> ConfigBlock;

class ConfigFile
{
public:
		ConfigFile();
		~ConfigFile();

		bool SetSource(const char* file, bool ignorecase = true);
		ConfigSetting* GetSetting(const char* Block, const char* Setting);

		bool GetString(const char* block, const char* name, std::string* value);
		std::string GetStringDefault(const char* block, const char* name, const char* def);
		std::string GetStringVA(const char* block, const char* def, const char* name, ...);
		bool GetString(const char* block, char* buffer, const char* name, const char* def, uint32 len);

		bool GetBool(const char* block, const char* name, bool* value);
		bool GetBoolDefault(const char* block, const char* name, const bool def);

		bool GetInt(const char* block, const char* name, int* value);
		int GetIntDefault(const char* block, const char* name, const int def);
		int GetIntVA(const char* block, int def, const char* name, ...);

		bool GetFloat(const char* block, const char* name, float* value);
		float GetFloatDefault(const char* block, const char* name, const float def);
		float GetFloatVA(const char* block, float def, const char* name, ...);

private:
		map<uint32, ConfigBlock> m_settings;
};

#endif
