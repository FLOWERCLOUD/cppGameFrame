
#include "MySqlCon.h"

MySqlCon::MySqlCon(const std::string& host, const std::string& port, const std::string& user, const std::string& password, const std::string& database):
	host_(host),
	port_or_socket_(port),
	user_(user),
	password_(password),
	database_(database),
	mysql_(NULL)
{
}

MySqlCon::~MySqlCon()
{
    /// Only close us if we're not operating
    //delete this;
	if (mysql_)
	{
		mysql_close(mysql_);
		//mysql_library_end();
	}
}

bool MySqlCon::open()
{
	MYSQL* mysqlInit = mysql_init(NULL);
	if (!mysqlInit)
	{
		return false;
	}

	int port;
	char const* unix_socket;
	mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");
	if (host_ == ".") // socket use option (Unix/Linux)
	{
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, reinterpret_cast<char const*>(&opt));
        host_ = "localhost";
        port = 0;
        unix_socket = port_or_socket_.c_str();
	}
    else                                                    // generic case
    {
        port = atoi(port_or_socket_.c_str());
        unix_socket = 0;
    }

    mysql_ = mysql_real_connect(mysqlInit,
    		host_.c_str(),
    		user_.c_str(),
    		password_.c_str(),
    		database_.c_str(),
			port,
			unix_socket,
			0);

    if (!mysql_)
    {
        mysql_close(mysqlInit);
        return false;
    }

    mysql_autocommit(mysql_, 1);

    // set connection properties to UTF8 to properly handle locales for different
    // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
    mysql_set_character_set(mysql_, "utf8");
    return true;
}

bool MySqlCon::query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, unsigned long* pRowCount, unsigned int* pFieldCount)
{
		if (!mysql_)
		{
			return false;
		}

		{
			if (mysql_query(mysql_, sql))
			{
				return false;
			}

			*pResult = mysql_store_result(mysql_);
			*pRowCount = mysql_affected_rows(mysql_);
			*pFieldCount = mysql_field_count(mysql_);
		}

		if (!*pResult )
		{
			return false;
		}

		*pFields = mysql_fetch_fields(*pResult);
		return true;
}

bool MySqlCon::execute(const char* sql)
{
    if (!mysql_)
    {
    	return false;
    }

    {
        if (mysql_query(mysql_, sql))
        {
            return false;
        }
    }

    return true;
}
