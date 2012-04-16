#include "htmlclient.h"

#include <curl/curl.h>


CHtmlClient::CHtmlClient()
:_url(NULL),_error(0),_maxLength(-1)
{
	_url = curl_easy_init();
}

CHtmlClient::~CHtmlClient()
{
	if( _url )
	{
		curl_easy_cleanup(_url);
	}
	_url = NULL;
}

void CHtmlClient::setMaxLength( int len )
{
	_maxLength = len;
}

int CHtmlClient::getMaxLength() const
{
	return _maxLength;
}

size_t mycurl_write_callback(char *ptr, size_t size, size_t nmemb, void *userp)
{
	CHtmlClient *client = static_cast<CHtmlClient*>(userp);
	if( client == NULL )
	{
		return -1;
	}
	if(size * nmemb < 1)
		return 0;

	if( !client->writeCallback( ptr , size , nmemb ) )
	{
		return -1;
	}
	return size * nmemb;
}

int CHtmlClient::loadUrl( const std::string& url , int timeOut )
{
	_bytesBuff.clear();

	CURL *purl = _url;

	if( purl == NULL )
	{
		return -1;
	} 
		
    // 禁用掉alarm超时
	curl_easy_setopt( purl, CURLOPT_NOSIGNAL, 1L);
	//DNS超时控制
	curl_easy_setopt( purl, CURLOPT_DNS_CACHE_TIMEOUT , 3600 );
	curl_easy_setopt( purl, CURLOPT_TIMEOUT, timeOut );

		
	curl_easy_setopt(purl, CURLOPT_WRITEFUNCTION, mycurl_write_callback );
	curl_easy_setopt(purl, CURLOPT_WRITEDATA, this );

	curl_easy_setopt(purl, CURLOPT_URL, url.c_str() );

	//curl_easy_setopt(purl, CURLOPT_COOKIEJAR,"cookiefile");
	//curl_easy_setopt(purl, CURLOPT_COOKIESESSION, true);
	
	CURLcode ret = curl_easy_perform( purl );
	_error = ret;
	printf("error: %d\n", _error);
    if( CURLE_OK != ret )
	{
		return -1;
	}
	return 0;
}

int CHtmlClient::post(const std::string& url, const std::string& postfield, int timeOut)
{
	_bytesBuff.clear();

	CURL *purl = _url;

	if( purl == NULL )
	{
		return -1;
	}

    // 禁用掉alarm超时
	curl_easy_setopt( purl, CURLOPT_NOSIGNAL, 1L);
	//DNS超时控制
	curl_easy_setopt( purl, CURLOPT_DNS_CACHE_TIMEOUT , 3600 );
	curl_easy_setopt( purl, CURLOPT_TIMEOUT, timeOut );


	curl_easy_setopt(purl, CURLOPT_WRITEFUNCTION, mycurl_write_callback );
	curl_easy_setopt(purl, CURLOPT_WRITEDATA, this );

	curl_easy_setopt(purl, CURLOPT_URL, url.c_str() );

	curl_easy_setopt(purl, CURLOPT_COOKIEJAR,"cookiefile");
	curl_easy_setopt(purl, CURLOPT_COOKIESESSION, true);

	curl_easy_setopt(purl, CURLOPT_POST, 1);//启用POST提交
	curl_easy_setopt(purl, CURLOPT_POSTFIELDS, postfield.c_str() ); //设置POST提交的字符串

	CURLcode ret = curl_easy_perform( purl );
	_error = ret;
	printf("error: %d\n", _error);
    if( CURLE_OK != ret )
	{
		return -1;
	}
	return 0;
}

bool CHtmlClient::writeCallback( void *ptr, size_t size, size_t nmemb )
{
	if( _maxLength > 0 )
	{
		if( _bytesBuff.getDataSize() + static_cast<int>(size * nmemb) > _maxLength )
		{
			return false;
		}
	}
	_bytesBuff.append( ptr ,  static_cast<int>(size * nmemb) );
	return true;
}

int CHtmlClient::getErrorCode()
{
	return _error;
}

const CBytesBuffer& CHtmlClient::getHtmlData() const
{
	return _bytesBuff;
}

int CHtmlClient::initialize()
{
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        return -1;
    }
	return 0;
}

int CHtmlClient::finalise()
{
	curl_global_cleanup();
	return 0;
}

