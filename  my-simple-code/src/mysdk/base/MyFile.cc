/*
 * MyFile.cc
 *
 *  Created on: 2011-10-24
 *    
 */

#include <mysdk/base/MyFile.h>

#include <mysdk/base/Logging.h>

#include <unistd.h>

namespace mysdk
{
	bool MyFile::Exists(const std::string& name)
	{
		return access(name.c_str(), F_OK) == 0;
	}

	bool MyFile::ReadFileToString(const std::string& name, std::string* output)
	{
		char buffer[1024];
		FILE* file = fopen(name.c_str(), "rb");
		if (file == NULL)
		{
			return false;
		}

		while (true)
		{
			size_t n = fread(buffer, 1, sizeof(buffer), file);
			if (n <= 0) break;
			output->append(buffer, n);
		}

		int error = ferror(file);
		if (fclose(file) != 0)
		{
			return false;
		}
		 return error == 0;
	}

	void MyFile::ReadFileToStringOrDie(const std::string& name, std::string* output)
	{
		int ret = ReadFileToString(name, output);
		if (ret != 0)
		{
			 LOG_SYSFATAL << "Could not read: " << name;
		}
	}

	void MyFile::WriteStringToFileOrDie(const std::string& contents, const std::string& name)
	{
		FILE* file = fopen(name.c_str(), "wb");
		if (file == NULL)
		{
			LOG_SYSFATAL << "fopen(" << name << ", \"wb\"): " << strerror(errno);
		}
		if (fwrite(contents.data(), 1, contents.size(), file) != contents.size())
		{
			LOG_SYSFATAL << "fwrite(" << name << "): " << strerror(errno);
		}
		if (fclose(file) == 0)
		{
			LOG_SYSFATAL << "fclose(" << name << "): " << strerror(errno);
		}
	}

	bool MyFile::CreateDir(const std::string& name, int mode)
	{
		return mkdir(name.c_str(), mode) == 0;
	}

	bool MyFile::RecursivelyCreateDir(const std::string& path, int mode)
	{
		if (CreateDir(path, mode))  return true;

		if (Exists(path)) return false;

		std::string::size_type slashpos = path.find_last_of('/');
		if (slashpos == std::string::npos)
		{
			return false;
		}

		return RecursivelyCreateDir(path.substr(0, slashpos), mode) && CreateDir(path, mode);
	}
}
