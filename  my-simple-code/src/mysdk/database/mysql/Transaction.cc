
#include <mysdk/database/mysql/Transaction.h>
#include <mysdk/database/mysql/MySQLBinds.h>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

using namespace mysdk;

#define MAX_QUERY_LEN 32*1024

Transaction::Transaction():
		cleanedUp_(false)
{

}

Transaction::~Transaction()
{
	cleanup();
}

//- Append a raw ad-hoc query to the transaction
void Transaction::append(const char* sql)
{
    SQLElementData data;
    data.type = SQL_ELEMENT_RAW;
    data.element.query = strdup(sql);
    queries_.push_back(data);
}

void Transaction::pAppend(const char* sql, ...)
{
    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, sql);
    vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
    va_end(ap);

    append(szQuery);
}

//- Append a prepared statement to the transaction
void Transaction::append(MySQLBinds* bind)
{
    SQLElementData data;
    data.type = SQL_ELEMENT_PREPARED;
    data.element.bind = bind;
    queries_.push_back(data);
}

void Transaction::cleanup()
{
    // This might be called by explicit calls to Cleanup or by the auto-destructor
    if (cleanedUp_)
        return;

    while (!queries_.empty())
    {
        SQLElementData const &data = queries_.front();
        switch (data.type)
        {
            case SQL_ELEMENT_PREPARED:
                delete data.element.bind;
            break;
            case SQL_ELEMENT_RAW:
                free(const_cast<char*>(data.element.query));
            break;
        }

        queries_.pop_front();
    }

    cleanedUp_ = true;
}
