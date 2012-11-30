
#ifndef MYSQL_QUERYRESULT_H
#define MYSQL_QUERYRESULT_H

#include <mysdk/base/Common.h>

#include <mysql.h>
#include <assert.h>

#include "Field.h"

class Field;
class ResultSet
{
public:
	ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
	~ResultSet();

	bool nextRow();
	uint64 getRowCount() const { return m_rowCount; }
	uint32 getFieldCount() const { return m_fieldCount; }

	Field* fetch() const {return m_currentRow; }
	const Field& operator[] (uint32 index) const
	{
		assert(index < m_fieldCount);
		return m_currentRow[index];
	}
protected:
    Field *m_currentRow;
    uint64 m_rowCount;
    uint32 m_fieldCount;

private:
    void cleanUp();
    MYSQL_RES *m_result;
    MYSQL_FIELD *m_fields;
};

#endif

