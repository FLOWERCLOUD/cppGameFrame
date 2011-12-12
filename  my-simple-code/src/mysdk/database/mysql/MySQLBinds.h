/*
 * MySQLBinds.h
 *
 *  Created on: 2011-11-5
 *    
 */

#ifndef MYSDK_DATABASE_MYSQL_MYSQLBINDS_H_
#define MYSDK_DATABASE_MYSQL_MYSQLBINDS_H_

#include <mysdk/base/Common.h>

#include <mysql.h>

namespace mysdk
{

enum USEFLAG
{
	NO_USE	=	0,
	BE_USE	=	1,
};

class MySQLBinds
{
public:
	MySQLBinds(uint32 index, uint8 paramCount);
	~MySQLBinds();

	uint32 getIndex() { return index_; }
	uint16 getParamCount() { return paramCount_; }
	MYSQL_BIND* getMysqlBind() { return mysqlBind_; }

	void setBool(const uint8 index, const bool value);
    void setUInt8(const uint8 index, const uint8 value);
    void setUInt16(const uint8 index, const uint16 value);
    void setUInt32(const uint8 index, const uint32 value);
    void setUInt64(const uint8 index, const uint64 value);
    void setInt8(const uint8 index, const int8 value);
    void setInt16(const uint8 index, const int16 value);
    void setInt32(const uint8 index, const int32 value);
    void setInt64(const uint8 index, const int64 value);
    void setFloat(const uint8 index, const float value);
    void setDouble(const uint8 index, const double value);
    void setString(const uint8 index, const char* value);
    void setString(const uint8 index, const char* value, const size_t valueLen);
private:
    bool checkValidIndex(uint8 index);
    void setValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned);
private:
	uint32 index_;
	uint16 paramCount_;
	MYSQL_BIND* mysqlBind_;
	uint8* paramSet_;
private:
	DISALLOW_COPY_AND_ASSIGN(MySQLBinds);
};

}

#endif /* MYSQLBINDS_H_ */
