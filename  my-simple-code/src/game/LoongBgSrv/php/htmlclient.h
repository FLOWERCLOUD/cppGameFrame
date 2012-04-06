#ifndef _HTML_CLINET_H_
#define _HTML_CLINET_H_
typedef void CURL;


#include "util/bytesbuffer.h"

#include <string>

class CHtmlClient
{
	public:
		CHtmlClient();
		~CHtmlClient();

		int loadUrl( const std::string& url , int timeOut = 10 );
		int post(const std::string& url, const std::string& postfield, int timeOut = 10);

		bool writeCallback( void *ptr, size_t size, size_t nmemb );

		const CBytesBuffer& getHtmlData() const;
		int getErrorCode();

		void setMaxLength( int len );
		int getMaxLength() const;

		static int initialize();
		static int finalise();

	private:
		void * _url;
		int _error;
		int _maxLength;
		CBytesBuffer _bytesBuff;
};

#endif
