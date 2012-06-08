
#ifndef URLENCODE_H_
#define URLENCODE_H_

/*
   URLEncode是这样编码的
   1。数字和字母不变。
   2。空格变为"+"号。
   3。其他被编码成"%"加上他们的ascii的十六进制，规律是这样的 (汉字 不参与这个编码)
   比如“啊”字 Ascii的十六进制是B0A1——>%B0%A1(Note:它是每个字节前加个%)。
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace ctool
{
#pragma GCC diagnostic ignored "-Wold-style-cast"
    inline bool URLEncode(const char* sIn, char* sOut, int sOutLen)
    {
        static const char szUnsafe[] = {"\"<>%\\^[]`+$,@:;/!#?=&\'|"};
        static const char szHexChr[] = {"0123456789ABCDEF"};

    	if (!sIn || !sOut || sOutLen <= 0)
    	{
    		printf("%s, %d \n", __FUNCTION__, __LINE__);
    		return false;
    	}

    	int len =  static_cast<int>(strlen(sIn));
    	if (sOutLen < len << 2)
    	{
    		printf("%s, %d sOutLen is less!!\n", __FUNCTION__, __LINE__);
    		return false;
    	}

    	int sOutIndex = 0;
    	for (int i = 0; i < len; i++)
    	{
    		char buf[4];
    		memset(buf, 0, 4);

    		char ch = (char)sIn[i];
    		if (isalnum( ch ) ||
    				NULL == strchr(szUnsafe, ch) )
    		{
    			buf[0] = ch;
    		}
    		else if (isspace(  ch ))
    		{
    			buf[0] = '+';
    		}
    		else
    		{
    			buf[0] = '%';
    			buf[1] = szHexChr[(int)((ch >> 4) & 0x0f)];
    			buf[2] = szHexChr[(int)(ch & 0x0f)];
    		}

    		int bufLen = static_cast<int>(strlen(buf));
    		for (int j = 0; j < bufLen; j++)
    		{
    			sOut[sOutIndex++] = buf[j];
    		}
    	}
    	sOut[sOutIndex] = 0;
    	return true;
    }
#pragma GCC diagnostic error "-Wold-style-cast"
}

#endif /* CURLENCODE_H_ */
