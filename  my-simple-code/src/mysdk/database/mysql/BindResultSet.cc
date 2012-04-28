/*
 * PreparedResultSet.cc
 *
 *  Created on: 2011-11-7
 *    
 */

#include <mysdk/database/mysql/BindResultSet.h>
#include <mysdk/base/Logging.h>

#include <string.h>

using namespace mysdk;

BindResultSet::BindResultSet(MYSQL_STMT* stmt, MYSQL_RES *result, uint64 rowCount, uint32 fieldCount) :
		rowCount_(rowCount),
		rowPosition_(0),
		fieldCount_(fieldCount),
		rBind_(NULL),
		stmt_(stmt),
		res_(result),
		isNull_(NULL),
		length_(NULL)
{
    if (!res_)
        return;

    rBind_ = new MYSQL_BIND[fieldCount_];
    isNull_ = new my_bool[fieldCount_];
    length_ = new unsigned long[fieldCount_];

    memset(isNull_, 0, sizeof(my_bool) * fieldCount_);
    memset(rBind_, 0, sizeof(MYSQL_BIND) * fieldCount_);
    memset(length_, 0, sizeof(unsigned long) * fieldCount_);

    //- This is where we store the (entire) resultset
    if (mysql_stmt_store_result(stmt_))
    {
    	LOG_TRACE << "mysql_stmt_store_result, cannot bind result from MySQL server. Error: " << mysql_stmt_error(stmt_);
        return;
    }

    //- This is where we prepare the buffer based on metadata
    uint32 i = 0;
    MYSQL_FIELD* field = mysql_fetch_field(res_);
    while (field)
    {
        size_t size = Field::sizeForType(field);

        rBind_[i].buffer_type = field->type;
        rBind_[i].buffer = new char[size];
        memset(rBind_[i].buffer, 0, size);
        rBind_[i].buffer_length = size;
        rBind_[i].length = &length_[i];
        rBind_[i].is_null = &isNull_[i];
        rBind_[i].error = NULL;
        rBind_[i].is_unsigned = field->flags & UNSIGNED_FLAG;

        ++i;
        field = mysql_fetch_field(res_);
    }

    //- This is where we bind the bind the buffer to the statement
    if (mysql_stmt_bind_result(stmt_, rBind_))
    {
        delete[] rBind_;
        delete[] isNull_;
        delete[] length_;
    	return;
    }

    rowCount_ = mysql_stmt_num_rows(stmt_);

    rows_.resize(uint32(rowCount_));
    while (_nextRow())
    {
        rows_[uint32(rowPosition_)] = new Field[fieldCount_];
        for (uint64 fIndex = 0; fIndex < fieldCount_; ++fIndex)
        {
            if (!*rBind_[fIndex].is_null)
            {
            	rows_[uint32(rowPosition_)][fIndex].setByteValue( rBind_[fIndex].buffer,
																												rBind_[fIndex].buffer_length,
																												rBind_[fIndex].buffer_type,
																												static_cast<uint32>(*rBind_[fIndex].length) );

            }
            else
            {
                switch (rBind_[fIndex].buffer_type)
                {
                    case MYSQL_TYPE_TINY_BLOB:
                    case MYSQL_TYPE_MEDIUM_BLOB:
                    case MYSQL_TYPE_LONG_BLOB:
                    case MYSQL_TYPE_BLOB:
                    case MYSQL_TYPE_STRING:
                    case MYSQL_TYPE_VAR_STRING:
                    	rows_[uint32(rowPosition_)][fIndex].setByteValue( "",
																		rBind_[fIndex].buffer_length,
																		rBind_[fIndex].buffer_type,
																		static_cast<uint32>(*rBind_[fIndex].length) );
                    break;
                    default:
                    	rows_[uint32(rowPosition_)][fIndex].setByteValue( 0,
															rBind_[fIndex].buffer_length,
															rBind_[fIndex].buffer_type,
                                                           static_cast<uint32>(*rBind_[fIndex].length) );
					}
				}
        }
        rowPosition_++;
    }
    rowPosition_ = 0;

    /// All data is buffered, let go of mysql c api structures
    cleanUp();
}

BindResultSet::~BindResultSet()
{
    for (int i = 0; i < static_cast<int>(rowCount_); ++i)
    {
    	delete[] rows_[i];
    }
}

bool BindResultSet::nextRow()
{
    /// Only updates the rowPosition_ so upper level code knows in which element
    /// of the rows vector to look
    if (++rowPosition_ >= rowCount_)
    {
    	return false;
    }

    return true;
}

bool BindResultSet::_nextRow()
{
    /// Only called in low-level code, namely the constructor
    /// Will iterate over every row of data and buffer it
    if (rowPosition_ >= rowCount_)
        return false;

    int retval = mysql_stmt_fetch( stmt_ );

    if (!retval || retval == MYSQL_DATA_TRUNCATED)
        retval = true;

    if (retval == MYSQL_NO_DATA)
        retval = false;

    return retval;
}

void BindResultSet::cleanUp()
{
    /// More of the in our code allocated sources are deallocated by the poorly documented mysql c api
    if (res_)
    {
    	mysql_free_result(res_);
    }

    freeBindBuffer();
    mysql_stmt_free_result(stmt_);

    delete[] rBind_;
    delete[] isNull_;
    delete[] length_;
}

void BindResultSet::freeBindBuffer()
{
    for (uint32 i = 0; i < fieldCount_; ++i)
    {
    	delete[] ( static_cast<char*>(rBind_[i].buffer) );
    }
}
