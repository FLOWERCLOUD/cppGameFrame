
#ifndef GAME_RCACHE_H_
#define GAME_RCACHE_H_

#include <mysdk/base/Common.h>
#include <mysdk/base/Singleton.h>
#include <mysdk/base/Logging.h>

#include <assert.h>
#include <string>
#include <vector>

#include "hiredis/hiredis.h"

namespace rcache {

	class Cache
	{
	public:
		static const int sPingRedisIntervalSecs = 10; // 10s 没隔10s ping 一下redis
	public:
		Cache();
		~Cache();
	public:
		bool Init();
		bool Connect();
		void Run(int curTime);
		void Shutdown();
		bool SetServerAddr(const std::string& ip, int port);

		bool Ping();
	public:
		void *RedisCommand(const char *format, ...);
		static void FreeReplyObject(redisReply* reply);
	private:
		void internalRun(int curTime);

		bool CheckReplyStatus(redisReply* reply,const char* def = "OK");
	public:
		bool ReConnect();
	private:
	    redisContext* m_redisContext;
		std::string m_redisIP;
		int m_redisPort;
		std::string m_Password;			//如果开启了auth则要发送此密码去验证
		int m_DBid;			//数据库ID
		int m_tLastPingRedis;
	};

}

#endif
