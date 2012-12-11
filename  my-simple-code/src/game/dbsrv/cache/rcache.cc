
#include <game/dbsrv/cache/rcache.h>

#include <strings.h>
#include <stdarg.h>

namespace rcache
{

Cache::Cache():
		m_redisContext(NULL),
		m_DBid(0),
		m_tLastPingRedis(0)
{
}

Cache::~Cache()
{

}

bool Cache::Init()
{
	return true;
}

bool Cache::SetServerAddr(const std::string& ip, int port)
{
	m_redisIP = ip;
	m_redisPort = port;
	return true;
}

bool Cache::Connect()
{
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    m_redisContext = redisConnectWithTimeout(m_redisIP.c_str(), m_redisPort, timeout);
    if (m_redisContext->err)
    {
    	printf("[redis] connection error: %d\n", m_redisContext->err);
    	return false;
    }

    //bool authResult = false;
	if (!m_Password.empty())
	{
		redisReply* reply = static_cast<redisReply*>(redisCommand(m_redisContext, "AUTH %s", m_Password.data()));
		if (!reply) return false;

		if (!CheckReplyStatus(reply))
		{
			printf("[redis] failed by password :%s\n", reply->str);
			freeReplyObject(reply);
			return false;
		}

		freeReplyObject(reply);
	}

	if (m_DBid)
	{
		redisReply* reply = static_cast<redisReply*> (redisCommand(m_redisContext, "SELECT %d", m_DBid));
		if (!reply) return false;

		if (!CheckReplyStatus(reply))
		{
			printf("[redis] failed by select db(%d) :%s\n", m_DBid, reply->str);
			freeReplyObject(reply);
			return false;
		}
		freeReplyObject(reply);
	}
	printf("[redis] successful connected %s :%d\n", m_redisIP.data(), m_redisPort);
    return true;
}

void Cache::Run(int curTime)
{
	internalRun(curTime);
}

void Cache::Shutdown()
{
	if (m_redisContext)
	{
		redisFree(m_redisContext);
	}
}

bool Cache::Ping()
{
	if(!m_redisContext)
	{
		ReConnect();
		return false;
	}

	redisReply* reply = static_cast<redisReply*> (redisCommand(m_redisContext, "PING"));
	if (reply)
	{
		if (CheckReplyStatus(reply, "PONG"))
		{
			freeReplyObject(reply);
			return true;
		}
		printf("[redis]Ping has an error occur:%s\n ", reply->str);
		freeReplyObject(reply);
	}
	printf("[redis] Ping to Server Have Some Problem,So Reconnect Now \n");
	//ping不通，重新建立起连接
	if (!ReConnect())
	{
		m_redisContext = NULL;
	}
	return false;
}

bool Cache::CheckReplyStatus(redisReply* reply, const char* def /*= "OK"*/)
{
	if (reply)
	{
		return (reply->type == REDIS_REPLY_STATUS && !strcasecmp(reply->str, def));
	}
	return false;
}

bool Cache::ReConnect()
{
	printf("[reids] Redis Connect Failed, now Reconnect \n");
	bool flag = Connect();
	if (!flag)
	{
    	redisFree(m_redisContext);
    	m_redisContext = NULL;
	}
	return flag;
}

void Cache::internalRun(int curTime)
{
	if (curTime - m_tLastPingRedis > sPingRedisIntervalSecs)
	{
		m_tLastPingRedis = curTime;
		Ping();
	}
}

void* Cache::RedisCommand(const char *format, ...)
{
	if (!m_redisContext)
	{
		fprintf(stderr, "Cache::RedisCommand  m_redisContext error!!\n");
		return NULL;
	}

    va_list ap;
    void *reply = NULL;
    va_start(ap,format);
    reply = redisvCommand(m_redisContext,format,ap);
    va_end(ap);
    return reply;
}

void Cache::FreeReplyObject(redisReply* reply)
{
	if (reply)
	{
		freeReplyObject(reply);
	}
}

}
