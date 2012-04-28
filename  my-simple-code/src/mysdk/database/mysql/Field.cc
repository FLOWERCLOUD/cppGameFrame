
#include <mysdk/database/mysql/Field.h>

#include <mysdk/base/Logging.h>

#include <string.h>

using namespace mysdk;

Field::Field()
{
	data.value = NULL;
	data.type = MYSQL_TYPE_NULL;
	data.length = 0;
}

Field::~Field()
{
	cleanUp();
}

void Field::setByteValue(const void* newValue, const size_t newSize, enum_field_types newType, uint32 length)
{
    if (data.value)
        cleanUp();

    // This value stores raw bytes that have to be explicitly casted later
    if (newValue)
    {
        data.value = new char[newSize];
        assert(data.value);
        memcpy(data.value, newValue, newSize);
        data.length = length;
    }
    data.type = newType;
    data.raw = true;
}

void Field::setStructuredValue(char* newValue, enum_field_types newType)
{
    if (data.value)
        cleanUp();

    // This value stores somewhat structured data that needs function style casting
    if (newValue)
    {
        size_t size = strlen(newValue);
        data.value = new char [size+1];
        strcpy(static_cast<char*> (data.value), newValue);
        data.length = static_cast<uint32>(size);
    }

    data.type = newType;
    data.raw = false;
}
