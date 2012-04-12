#include <mysdk/base/SysInfo.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cstdlib>

long SysInfo::GetCPUCount()
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

unsigned long long SysInfo::GetCPUUsage()
{
	rusage usage;

	if (getrusage(RUSAGE_SELF, &usage) != 0)
		return 0;

	unsigned long long k = usage.ru_utime.tv_sec * 1000000
			+ usage.ru_utime.tv_usec;
	unsigned long long u = usage.ru_stime.tv_sec * 1000000
			+ usage.ru_stime.tv_usec;

	return (k + u);
}

float SysInfo::GetRAMUsage()
{
	std::ifstream memf;
	std::string line;
	std::string value;
	std::stringstream fname;
	char line2[100];
	float memusage;
	unsigned long pos = 0;
	unsigned long pid = getpid();

	fname << "/proc/" << pid << "/status";

	memf.open(fname.str().c_str());

	do
	{
		pos = std::string::npos;
		memf.getline(line2, 100);
		line.assign(line2);
		pos = line.find("VmRSS");
	} while (pos == std::string::npos);

	pos = line.find(" ");
	++pos;

	value = line.substr(pos, std::string::npos);

	memusage = static_cast<float> (atof(value.c_str()));

	memf.close();

	memusage *= 1024.0f;

	return memusage;
}

unsigned long long SysInfo::GetTickCount()
{
	timeval Time;

	if (gettimeofday(&Time, NULL) != 0)
		return 0;

	return (Time.tv_sec * 1000) + (Time.tv_usec / 1000);
}
