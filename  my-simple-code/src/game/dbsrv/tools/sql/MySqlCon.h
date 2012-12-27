
#ifndef GAME_MYSQLCON_H_
#define GAME_MYSQLCON_H_

#include <string>
#include <mysql.h>

class MySqlCon
{
public:
	MySqlCon(const std::string& host, const std::string& port, const std::string& user, const std::string& password, const std::string& database);
	~MySqlCon();

	bool open();
	bool query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, unsigned long* pRowCount, unsigned int* pFieldCount);
	bool execute(const char* sql);
private:
    std::string host_;
    std::string port_or_socket_;
    std::string user_;
    std::string password_;
    std::string database_;

    MYSQL* mysql_; // MySQL Handle
};

#endif
