#ifndef _STR_UTIL_H
#define _STR_UTIL_H

#include <string>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <iconv.h>
#include <utility>
#include <ctype.h>
#include <vector>

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Buffer.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTime.h>
#include <Poco/Format.h>

namespace util
{

	using std::string;
	using std::pair;
	using std::vector;
	using namespace Poco;

	const int utf8_table1[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};
	const unsigned char utf8_table2[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
				3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };

	inline int atoi(string a)
	{
		return std::atoi(a.c_str());
	}

	inline string itoa(int i)
	{
		char buf[64];
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf)-1, "%d", i);
		return buf;
	}

	inline string tostr(const char* str)
	{
		return str ? str : "";
	}

	inline string tostr(vector<int>& vec, string delims = " ")
	{
		string s = "";
		if (vec.empty())
			return s;
		for (int i = 0; i < int(vec.size()) - 1; ++i)
			s += itoa(vec[i]) + delims;
		s += itoa(vec.back());
		return s;
	}

	template<typename _OutputIter>
	inline void split(const string& str, _OutputIter result, string delims = " \t")
	{
		int i = 0;
		int size = str.size();
		while (i < size)
		{
			while (i<size and (delims.find(str[i]) != string::npos)) i++;
			int j = i;
			while (j<size and (delims.find(str[j]) == string::npos)) j++;
			if (i != j) 
				*(result++) = str.substr(i, j-i);
			i = j;
		}
	}

	inline int split(const string& source, const string& delimiter, vector<string>& vec, bool cleanempty = true)
	{
		if (source.empty()) return 0;

		size_t pos = 0;
		size_t n;
		string str = source;
		str.append(delimiter);

		string sub;
		while ((n=str.find(delimiter, pos)) != string::npos)
		{
			sub = str.substr(pos, n-pos);
			if (!cleanempty || !sub.empty())
				vec.push_back(str.substr(pos, n-pos));
			pos = n + delimiter.size();
		}

		return int(vec.size());
	}

	template<typename _InputIter>
	inline string join(_InputIter begin, _InputIter end, string joint = " ")
	{
		string str = "";
		for (_InputIter i = begin; i != end; ++i)
		{
			str += *i;
			_InputIter j = i;
			j++;
			if (j != end)
				str += joint;
		}
		return str;
	}


	inline void toupper(string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::toupper);
	}

	inline void tolower(string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))std::tolower);
	}

	inline string from_to(const string& src, const string& enc_from, const string& enc_to)
	{
		if (enc_from == enc_to)
			return src;

		iconv_t cd = iconv_open(enc_to.c_str(), enc_from.c_str());

		char dest[4096];

		char * in = const_cast<char*>(src.c_str());
		char * out = dest;
		size_t ileft = strlen(in);
		size_t oleft = sizeof(dest);

		iconv(cd, &in, &ileft, &out, &oleft);

		iconv_close(cd);

		return string(dest, sizeof(dest) - oleft);
	}

	inline string convert(const string& src, bool gbk_to_utf8 = false)
	{
		if (gbk_to_utf8)
			return from_to(src, "gbk", "utf8");
		else
			return from_to(src, "utf8", "gbk");
	}
	
	/// split an address (host:port) into host and port.
	inline pair<string, u_short> split_addr(const string& addr)
	{
		int p = addr.find_last_of(':');
		u_short port = 0;
		string host = addr;
		if (p != (int)string::npos) {
			port = (u_short)atoi(addr.substr(p+1));
			host = addr.substr(0, p);
		}
		return make_pair(host, port);
	}
	// hash from string to int
	inline int strhash2int(const string& str)
	{
		unsigned int buckets = 10000;
		unsigned int tot = 0;
		for(int i=0; i< (int)str.length(); i++)
			tot += (tot<<5) + tot + (unsigned int)str[i];
		tot = tot%buckets;
		return tot;	
	}	

	// Scans a string encoded in utf-8 to verify that it contains
	// only valid sequences. It will return 1 if the string contains
	// only legitimate encoding sequences; otherwise it will return 0;
	// From 'Secure Programming Cookbook', John Viega & Matt Messier, 2003
	inline bool isutf8(const string& str)
	{       
		int nb;
		const unsigned char *input = (unsigned char*)str.c_str();
		const unsigned char *c = input;
		for (c = input;  *c;  c +=(nb + 1))  
		{   
			if (!(*c & 0x80)) nb = 0;
			else if ((*c & 0xc0) == 0x80) return false;
			else if ((*c & 0xe0) == 0xc0) nb = 1;
			else if ((*c & 0xf0) == 0xe0) nb = 2;
			else if ((*c & 0xf8) == 0xf0) nb = 3;
			else if ((*c & 0xfc) == 0xf8) nb = 4;
			else if ((*c & 0xfe) == 0xfc) nb = 5;

			int i;
			for (i = 0; i < nb; ++i)
			{
				if((*(c+i+1) & 0xC0) != 0x80)
					break;
			}
			if (i < nb)
				break;

			if (nb > 0)
			{
				if ((*(c + nb) & 0xc0) != 0x80)
					return false;
			}
		}
		return true;
	}

	inline string truncate_utf8(const char* eptr, int charlen)
	{
	    int totallength = 0;
	    int totalbytelength=0;
	    int len = 1;
	    unsigned char c;
	    for (const char* p = eptr; *p != 0 && totallength<charlen; p+=len)
	    {
	        len = 1;
	        c = *p;
	        if ((c & 0xc0) == 0xc0)
	        {
	            int gcaa = utf8_table2[c & 0x3f];  /* Number of additional bytes */
	            //int gcss = 6*gcaa;
				int i;
				for (i = 0; i < gcaa; i++)
				{
					if((p[i + 1] & 0xC0) != 0x80)
						break;
				}
				if (i < gcaa)
					break;

	            len += gcaa;
	        }
	        totalbytelength += len;
	        totallength++;
	    }
	    return string(eptr, 0, totalbytelength);
	}

	inline bool isalnum(const string& str)
	{
		if (str.length() == 0)
			return false;

		for(int i=0; i<(int)str.length(); i++)
		{
			char c = str[i];
			if (!std::isalnum(c))
				return false;
		}
		return true;

	}

	inline bool isdigit(const string& str)
	{
		if (str.length() == 0)
			return false;

		for(int i=0; i<(int)str.length(); i++)
		{
			char c = str[i];
			if (!std::isdigit(c))
				return false;
		}
		return true;

	}

}


#endif
