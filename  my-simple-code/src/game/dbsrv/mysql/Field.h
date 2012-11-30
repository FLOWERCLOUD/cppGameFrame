
#ifndef MYSQL_FIELD_H
#define MYSQL_FIELD_H

#include <mysdk/base/Common.h>
#include <mysdk/base/Logging.h>

#include <mysql.h>
#include <stdlib.h>
#include <string>

using namespace mysdk;

#define MYSDK_DEBUG

class Field
{
public:
	Field();
    ~Field();

public:
	bool getBool() const
	{
		return getUInt8() == 1 ? true : false;
	}

	uint8 getUInt8() const
	{
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getUInt8() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<uint8* >(data.value);

		return static_cast<uint8>(atol( static_cast<char*>(data.value) ));
	}

	int8 getInt8() const
	{
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt8() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<int8*>(data.value);

		return static_cast<int8>(atol( static_cast<char*>(data.value) ));
	}

    uint16 getUInt16() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getUInt16() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<uint16*>(data.value);

		return static_cast<uint16>(atol( static_cast<char*>(data.value) ));
    }

    int16 getInt16() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt16() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<int16*>(data.value);

		return static_cast<int16>(atol( static_cast<char*>(data.value) ));
    }

    uint32 getUInt32() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getUInt32() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<uint32*>(data.value);

		return static_cast<uint32>(atol( static_cast<char*>(data.value) ));
    }

    int32 getInt32() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt32() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<int32*>(data.value);

		return static_cast<int32>(atol( static_cast<char*>(data.value) ));
    }

    uint64 getUInt64() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getUInt64() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<uint64*>(data.value);

		return static_cast<uint64>(atol( static_cast<char*>(data.value) ) );
    }

    int64 getInt64() const
    {
		if (!data.value) return 0;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt64() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<int64*>(data.value);

		return static_cast<int64>(atol( static_cast<char*>(data.value) ));
    }

    float getFloat() const
    {
		if (!data.value) return 0.0f;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt64() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<float*>(data.value);

		return static_cast<float>(atof( static_cast<char*>(data.value) ));
    }

    double getDouble() const
    {
		if (!data.value) return 0.0f;
#ifdef MYSDK_DEBUG
		if (!isNumeric())
		{
			LOG_INFO << "Error: getInt64() on non-numeric field.";
			return 0;
		}
#endif
		if (data.raw) return *reinterpret_cast<double*>(data.value);

		return static_cast<double>(atof( static_cast<char*>(data.value) ));
    }

    const char* getCString() const
    {
    	if (!data.value) return NULL;
#ifdef MYSDK_DEBUG
		if (isNumeric())
		{
			LOG_INFO << "Error: getCString() on non-numeric field.";
			return NULL;
		}
#endif
		return static_cast<const char*>(data.value);
    }

    std::string getString() const
    {
        if (!data.value)
            return "";

        if (data.raw)
        {
            const char* string = getCString();
            if (!string)
                string = "";
            return std::string(string, data.length);
        }
        return std::string( static_cast<char*>(data.value) );
    }

public:
    #pragma pack(1)
    struct
    {
        uint32 length;          // Length (prepared strings only)
        void* value;            // Actual data in memory
        enum_field_types type;  // Field type
        bool raw;               // Raw bytes? (Prepared statement or ad hoc)
     } data;
    #pragma pack()


     void setByteValue(const void* newValue, const size_t newSize, enum_field_types newType, uint32 length);
     void setStructuredValue(char* newValue, enum_field_types newType);

     void cleanUp()
     {
         delete[] (static_cast<char*>(data.value));
         data.value = NULL;
     }

     static size_t sizeForType(MYSQL_FIELD* field)
     {
         switch (field->type)
         {
             case MYSQL_TYPE_NULL:
                 return 0;
             case MYSQL_TYPE_TINY:
                 return 1;
             case MYSQL_TYPE_YEAR:
             case MYSQL_TYPE_SHORT:
                 return 2;
             case MYSQL_TYPE_INT24:
             case MYSQL_TYPE_LONG:
             case MYSQL_TYPE_FLOAT:
                 return 4;
             case MYSQL_TYPE_DOUBLE:
             case MYSQL_TYPE_LONGLONG:
             case MYSQL_TYPE_BIT:
                 return 8;

             case MYSQL_TYPE_TIMESTAMP:
             case MYSQL_TYPE_DATE:
             case MYSQL_TYPE_TIME:
             case MYSQL_TYPE_DATETIME:
                 return sizeof(MYSQL_TIME);

             case MYSQL_TYPE_TINY_BLOB:
             case MYSQL_TYPE_MEDIUM_BLOB:
             case MYSQL_TYPE_LONG_BLOB:
             case MYSQL_TYPE_BLOB:
             case MYSQL_TYPE_STRING:
             case MYSQL_TYPE_VAR_STRING:
                 return field->length + 1; //field->max_length + 1;

             case MYSQL_TYPE_DECIMAL:
             case MYSQL_TYPE_NEWDECIMAL:
                 return 64;

             case MYSQL_TYPE_GEOMETRY:
             /*
             Following types are not sent over the wire:
             MYSQL_TYPE_ENUM:
             MYSQL_TYPE_SET:
             */
             default:
            	 LOG_INFO << " SQL::SizeForType(): invalid field type " << uint32(field->type);
                 return 0;
         }
     }

     bool isNumeric() const
     {
         return (data.type == MYSQL_TYPE_TINY ||
                 data.type == MYSQL_TYPE_SHORT ||
                 data.type == MYSQL_TYPE_INT24 ||
                 data.type == MYSQL_TYPE_LONG ||
                 data.type == MYSQL_TYPE_FLOAT ||
                 data.type == MYSQL_TYPE_DOUBLE ||
                 data.type == MYSQL_TYPE_LONGLONG );
     }
};

#endif
