
#ifndef GAME_UTIL_H_
#define GAME_UTIL_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Timestamp.h>
#include <mysdk/base/Logging.h>

#include <vector>

#include <sys/time.h>
#include <stdlib.h>

using namespace mysdk;

#define TIME_FUNCTION_CALL(p, t) \
{       Timestamp oldTimeStamp = Timestamp::now(); \
        (p); \
        Timestamp newTimeStamp = Timestamp::now(); \
        double diff = timeDifference(newTimeStamp, oldTimeStamp); \
        if (diff > t)\
        LOG_WARN << "[YOUHUA] call function: " << #p << " , time[s] = " << diff; \
}


std::vector<std::string> StrSplit(const std::string & src, const std::string & sep);

#endif /* UTIL_H_ */
