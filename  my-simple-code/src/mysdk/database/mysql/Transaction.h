
#ifndef MYSDK_DATABASE_MYSQL_TRANSACTION_H
#define MYSDK_DATABASE_MYSQL_TRANSACTION_H

#include <mysdk/base/Common.h>

#include <list>

namespace mysdk
{
class MySQLBinds;

//- Union that holds element data
union SQLElementUnion
{
    MySQLBinds* bind;
    const char* query;
};

//- Type specifier of our element data
enum SQLElementDataType
{
    SQL_ELEMENT_RAW,
    SQL_ELEMENT_PREPARED,
};

//- The element
struct SQLElementData
{
    SQLElementUnion element;
    SQLElementDataType type;
};

class Transaction
{
public:
	Transaction();
    ~Transaction();

    void append(MySQLBinds* bind);
    void append(const char* sql);
    void pAppend(const char* sql, ...);

    size_t getSize() const { return queries_.size(); }
    const std::list<SQLElementData> getQueries() const { return queries_;}
protected:
    void cleanup();
    std::list<SQLElementData> queries_;

private:
    bool cleanedUp_;
private:
	DISALLOW_COPY_AND_ASSIGN(Transaction);
};

}

#endif
