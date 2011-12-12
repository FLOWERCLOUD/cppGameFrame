/*
 * MyFile.h
 *
 *  Created on: 2011-10-24
 *    
 */

#ifndef MYSDK_BASE_MYFILE_H_
#define MYSDK_BASE_MYFILE_H_

#include <mysdk/base/Common.h>

#include <string>

namespace mysdk
{
	const int DEFAULT_FILE_MODE = 0777;

	class MyFile
	{
	public:
		static bool Exists(const std::string& name);
		static bool ReadFileToString(const std::string& name, std::string* output);
		static bool ReadFileToStringOrDie(const std::string& name, std::string* output);
		static void WriteStringToFileOrDie(const std::string& contents, const std::string& name);
		static bool CreateDir(const std::string& name, int mode);
		static bool RecursivelyCreateDir(const std::string& path, int mode);
	private:
		DISALLOW_COPY_AND_ASSIGN(MyFile);
	};
}

#endif /* MYFILE_H_ */
