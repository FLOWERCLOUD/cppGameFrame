
#ifndef MYSDK_DATABASE_MYSQL_BINDRESULTSET_H
#define MYSDK_DATABASE_MYSQL_BINDRESULTSET_H

#include <mysdk/base/Common.h>
#include <mysdk/database/mysql/Field.h>

#include "mysql.h"

#include <assert.h>
#include <vector>

namespace mysdk
{
class BindResultSet
{
public:
	BindResultSet(MYSQL_STMT* stmt, MYSQL_RES *result, uint64 rowCount, uint32 fieldCount);
	~BindResultSet();

	bool nextRow();
	uint64 getRowCount() const { return rowCount_; }
	uint32 getFieldCount() const { return fieldCount_; }

    Field* fetch() const
    {
        assert(rowPosition_ < rowCount_);
        return rows_[uint32(rowPosition_)];
    }

    const Field& operator[] (uint32 index) const
    {
    	assert(rowPosition_ < rowCount_);
    	assert(index < fieldCount_);
        return rows_[uint32(rowPosition_)][index];
    }

protected:
    uint64 rowCount_;
    uint64 rowPosition_;
    std::vector<Field*> rows_;
    uint32 fieldCount_;

private:
    MYSQL_BIND*  rBind_;
    MYSQL_STMT* stmt_;
    MYSQL_RES* res_;

    my_bool* isNull_;
    unsigned long* length_;

    void freeBindBuffer();
    void cleanUp();
    bool _nextRow();

private:
	DISALLOW_COPY_AND_ASSIGN(BindResultSet);
};

}

#endif /* PREPAREDRESULTSET_H_ */
