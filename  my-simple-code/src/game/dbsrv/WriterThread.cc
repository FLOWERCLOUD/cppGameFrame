
#include "WriterThread.h"

#include <game/dbsrv/config/ConfigMgr.h>
#include <game/dbsrv/LogThread.h>
#include <game/dbsrv/WriterThreadPool.h>

WriterThread::WriterThread(int id):
	id_(id),
	thread_(std::tr1::bind(&WriterThread::threadHandler, this), "WriterThread")
{

}

WriterThread::~WriterThread()
{

}

void WriterThread::start()
{
	// mysql
	std::string host = sConfigMgr.MainConfig.GetStringDefault("mysql", "host", "192.168.1.6");
	std::string port_or_socket = sConfigMgr.MainConfig.GetStringDefault("mysql", "port_or_socket", "3306");
	std::string user = sConfigMgr.MainConfig.GetStringDefault("mysql", "user", "root");
	std::string password = sConfigMgr.MainConfig.GetStringDefault("mysql", "password", "4399mysql#CQPZM");
	std::string database = sConfigMgr.MainConfig.GetStringDefault("mysql", "database", "newkabu");
	MySQLConnectionInfo coninfo(host, port_or_socket, user, password, database);
	mysql_.setConnectionInfo(coninfo);

	mysql_.open();
	thread_.start();
}

void WriterThread::stop()
{
	WriterThreadParam param;
	param.Type = WRITERTHREAD_STOP;
	queue_.put(param);
	thread_.join();

	mysql_.close();
}

void WriterThread::push(WriterThreadParam& param)
{
	queue_.put(param);
}

size_t WriterThread::getQueueSize()
{
	return queue_.size();
}

static size_t saveSqlNum = 0;
void WriterThread::threadHandler()
{
	LOGEX_INFO("WriterThread::threadHandler start... id[%d]", id_);
	while (true)
	{
		WriterThreadParam param = queue_.take();

		static size_t warn_queue_size = sConfigMgr.MainConfig.GetIntDefault("writerthread", "warn_queue_size", 1000);
		static size_t max_writesql_num = sConfigMgr.MainConfig.GetIntDefault("writerthread", "max_writesql_num", 100);

		if (param.Type == WRITERTHREAD_CMD)
		{
			handler(param);
			// 每保存max_writesql_num条sql 就sleep 1 s 一下吧， 防止mysql io拥堵
			if (saveSqlNum++ >= max_writesql_num)
			{
				saveSqlNum = 0;
				usleep(100);
			}
		}
		else if (param.Type == WRITERTHREAD_PING)
		{
			mysql_.ping();

			size_t queue_size = queue_.size();
			if (queue_size >= warn_queue_size)
			{
				LOGEX_WARN("write thread queue size too larger, size: %d, threadid: %d", queue_size, id_);
			}
		}
		else if (param.Type == WRITERTHREAD_STOP)
		{
			break;
		}
	}
	LOGEX_INFO("WriterThread::threadHandler stop... id[%d]", id_);
}

void WriterThread::handler(WriterThreadParam& param)
{
	char* sql = static_cast<char*>(param.sql);

	LOGEX_DEBUG("WriterThread::handler, sql: %s",sql);
	if (!mysql_.execute(sql, param.length))
	{
		// 保存到数据库失败~~~
		// 怎么处理呢？？？
		// 现在的处理方法是 保存保存不了的key到某个文件中
		LOGEX_RAW("%s", param.key);
	}
	// 释放掉这个内存空间啦
	sWriterThreadPool.DeleteSqlBuf(sql);
}
