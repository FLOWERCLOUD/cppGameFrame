
#include <mysdk/database/mysql/QueryResult.h>

using namespace mysdk;

ResultSet::ResultSet(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount):
		m_rowCount(rowCount),
		m_fieldCount(fieldCount),
		m_result(result),
		m_fields(fields)
{
    m_currentRow = new Field[m_fieldCount];
    assert(m_currentRow);
}

ResultSet::~ResultSet()
{
    cleanUp();
}

bool ResultSet::nextRow()
{
    MYSQL_ROW row;

    if (!m_result)
    {
    	return false;
    }

    row = mysql_fetch_row(m_result);
    if (!row)
    {
        cleanUp();
        return false;
    }

    for (uint32 i = 0; i < m_fieldCount; i++)
    {
    	m_currentRow[i].setStructuredValue(row[i], m_fields[i].type);
    }

    return true;
}

void ResultSet::cleanUp()
{
    if (m_currentRow)
    {
        delete[] m_currentRow;
        m_currentRow = NULL;
    }

    if (m_result)
    {
        mysql_free_result(m_result);
        m_result = NULL;
    }
}
