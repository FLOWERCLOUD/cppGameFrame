/*
 * Logging_unittest.cc
 *
 *  Created on: 2011-9-14
 *    
 */

#include <mysdk/base/Logging.h>

#include <stdio.h>

using namespace mysdk;
int main()
{
	LOG_TRACE << "LogTrace";
	LOG_DEBUG << "LogDebug";
	LOG_INFO << "LogInfo";
	LOG_WARN << "LogWarn";
	LOG_ERROR << "LogError";
	LOG_SYSERR << "LogSysErr";
	//LOG_FATAL << "LogFatal";
	//LOG_SYSFATAL << "LogSysFatal";
	return 0;
}
