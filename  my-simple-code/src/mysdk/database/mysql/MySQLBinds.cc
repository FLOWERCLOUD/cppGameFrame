/*
 * MySQLBinds.cc
 *
 *  Created on: 2011-11-5
 *    
 */

#include <mysdk/database/mysql/MySQLBinds.h>

#include <mysdk/base/Logging.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

using namespace mysdk;

MySQLBinds::MySQLBinds(uint32 index, uint8 paramCount):
		index_(index),
		paramCount_(paramCount)
{
	mysqlBind_ = new MYSQL_BIND[paramCount];
	paramSet_ = new uint8[paramCount];
	memset(mysqlBind_, 0, sizeof(MYSQL_BIND) * paramCount);
	memset(paramSet_, 0, sizeof(uint8) * paramCount);
}

MySQLBinds::~MySQLBinds()
{
	for (uint8 i = 0; i < paramCount_; i++)
	{
	    char* buffer = static_cast<char*> (mysqlBind_[i].buffer);
	    if (buffer)
	    {
	    	delete[] buffer;
	    }

	    enum_field_types type = mysqlBind_[i].buffer_type;
	    if (type == MYSQL_TYPE_VAR_STRING)
	    {
	    	delete mysqlBind_[i].length;
	    }
	}

	if (mysqlBind_)
	{
		delete[] mysqlBind_;
	}

	if (paramSet_)
	{
		delete[] paramSet_;
	}
}

void MySQLBinds::setBool(const uint8 index, const bool value)
{
    setUInt32(index, value);
}

void MySQLBinds::setUInt8(const uint8 index, const uint8 value)
{
    setUInt32(index, value);
}

void MySQLBinds::setUInt16(const uint8 index, const uint16 value)
{
    setUInt32(index, value);
}

void MySQLBinds::setUInt32(const uint8 index, const uint32 value)
{
    assert(checkValidIndex(index));
    paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_LONG,  &value, sizeof(uint32), true);
}

void MySQLBinds::setUInt64(const uint8 index, const uint64 value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_LONGLONG, &value, sizeof(uint64), true);
}

void MySQLBinds::setInt8(const uint8 index, const int8 value)
{
    setInt32(index, value);
}

void MySQLBinds::setInt16(const uint8 index, const int16 value)
{
    setInt32(index, value);
}

void MySQLBinds::setInt32(const uint8 index, const int32 value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_LONG, &value, sizeof(int32), false);
}

void MySQLBinds::setInt64(const uint8 index, const int64 value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_LONGLONG, &value, sizeof(int64), false);
}

void MySQLBinds::setFloat(const uint8 index, const float value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_FLOAT, &value, sizeof(float), (value > 0.0f));
}

void MySQLBinds::setDouble(const uint8 index, const double value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    setValue(param, MYSQL_TYPE_DOUBLE, &value, sizeof(double), (value > 0.0f));
}

void MySQLBinds::setString(const uint8 index, const char* value, const size_t valueLen)
{
	assert(checkValidIndex(index) && valueLen == strlen(value));

	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];

    param->buffer_type = MYSQL_TYPE_VAR_STRING;
    param->buffer = new char[valueLen + 1];
    param->buffer_length = valueLen + 1;
    param->is_null_value = 0;
    param->length = new unsigned long(valueLen);

    memcpy(param->buffer, value, valueLen);
}

void MySQLBinds::setString(const uint8 index, const char* value)
{
	assert(checkValidIndex(index));
	paramSet_[index] = true;
    MYSQL_BIND* param = &mysqlBind_[index];
    size_t len = strlen(value) + 1;
    param->buffer_type = MYSQL_TYPE_VAR_STRING;
    param->buffer = new char[len];
    param->buffer_length = len;
    param->is_null_value = 0;
    param->length = new unsigned long(len-1);

    memcpy(param->buffer, value, len);
}

void MySQLBinds::setValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned)
{
    param->buffer_type = type;
    param->buffer = new char[len];
    param->buffer_length = 0;
    param->is_null_value = 0;
    param->length = NULL;               // Only != NULL for strings
    param->is_unsigned = isUnsigned;

    memcpy(param->buffer, value, len);
}

bool MySQLBinds::checkValidIndex(uint8 index)
{
    if (index >= paramCount_)
    {
    	return false;
    }

    if (paramSet_[index] == BE_USE)
    {
    	LOG_INFO << "Prepared Statement (id: "
    			<< index_
    			<< " ) trying to bind value on already bound index ( "
    			<< index << " ). ";
    }
    return true;
}
