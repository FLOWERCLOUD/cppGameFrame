/*
 * MySQLConnectionInfo.h
 *
 *  Created on: 2011-11-4
 *    
 */

#ifndef MYSDK_DATABASE_MYSQL_MYSQLCONNECTIONINFO_H_
#define MYSDK_DATABASE_MYSQL_MYSQLCONNECTIONINFO_H_

#include <string>

namespace mysdk
{

enum ConnectionFlags
{
    CONNECTION_ASYNC = 0x1,
    CONNECTION_SYNCH = 0x2,
};

class MySQLConnectionInfo
{
public:
    MySQLConnectionInfo() {}
    MySQLConnectionInfo(const std::string& host,
    		const std::string& port_or_socket,
    		const std::string& user,
    		const std::string& password,
    		const std::string& database):
    			host_(host),
    			port_or_socket_(port_or_socket),
    			user_(user),
    			password_(password),
    			database_(database)
    {
    }

    void setHost(const std::string& host) { host_ = host; }
    const std::string& getUser() const { return user_;}
    const std::string& getPassword() const { return password_; }
    const std::string& getDatabase() const { return database_; }
    const std::string& getHost() const { return host_; }
    const std::string& getPortOrSocket() const { return port_or_socket_; }
private:
    std::string host_;
    std::string port_or_socket_;
    std::string user_;
    std::string password_;
    std::string database_;
};

}

#endif /* MYSQLCONNECTIONINFO_H_ */
