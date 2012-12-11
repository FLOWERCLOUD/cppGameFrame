
#include "Util.h"

std::vector<std::string> StrSplit(const std::string & src, const std::string & sep)
{
	std::vector<std::string> r;
	std::string s;
	for(std::string::const_iterator i = src.begin(); i != src.end(); ++i)
	{
		if(sep.find(*i) != string::npos)
		{
			if(s.length()) r.push_back(s);
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if(s.length()) r.push_back(s);
	return r;
}
