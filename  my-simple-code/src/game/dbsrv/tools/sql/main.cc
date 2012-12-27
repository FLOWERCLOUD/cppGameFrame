

#include <game/dbsrv/ProtoImporter.h>

#include "MySqlCon.h"
#include "ToolsConfigMgr.h"

#include <vector>
#include <string>
#include <sstream>

static std::string pbtypetosqltype(const google::protobuf::FieldDescriptor* fieldDescriptor)
{
	if (fieldDescriptor->is_repeated())
	{
		return "blob";
	}

	google::protobuf::FieldDescriptor::Type type = fieldDescriptor->type();
	if (type == google::protobuf::FieldDescriptor::TYPE_INT32)
	{
		return "int(10)";
	}
	else if (type == google::protobuf::FieldDescriptor::TYPE_BYTES)
	{
		return "blob";
	}
	else if (type == google::protobuf::FieldDescriptor::TYPE_STRING)
	{
		return "varchar(100)";
	}
	else if (type == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
	{
		return "blob";
	}
	return "";
}
int main(int argc, char* argv[])
{
	if(!sToolsConfigMgr.MainConfig.SetSource("def.conf", true))
	{
		fprintf(stderr, "read conf file error\n");
		return -1;
	}

	// mysql
	std::string host = sToolsConfigMgr.MainConfig.GetStringDefault("mysql", "host", "192.168.1.6");
	std::string port_or_socket = sToolsConfigMgr.MainConfig.GetStringDefault("mysql", "port_or_socket", "3306");
	std::string user = sToolsConfigMgr.MainConfig.GetStringDefault("mysql", "user", "root");
	std::string password = sToolsConfigMgr.MainConfig.GetStringDefault("mysql", "password", "4399mysql#CQPZM");
	std::string database = sToolsConfigMgr.MainConfig.GetStringDefault("mysql", "database", "newkabu");

	MySqlCon mysql(host, port_or_socket, user, password, database);

	if (!mysql.open())
	{
		fprintf(stderr, "con mysql error\n");
		return -1;
	}

	const char* sql = "DESCRIBE user2";
	MYSQL_RES* res;
	MYSQL_FIELD* fields;
	unsigned long rowCount;
	unsigned int fieldCound;
	if (!mysql.query(sql, &res, &fields, &rowCount, &fieldCound))
	{
		fprintf(stderr, " mysql query error\n");
		return -1;
	}

	std::vector<std::string> tablename(rowCount);
	for(unsigned long i = 0; i < rowCount; i++)
	{
        MYSQL_ROW row = mysql_fetch_row(res);
        tablename[i] = row[0];
	}

	bool flag = sProtoImporter.Import("test.proto");
	if (!flag)
	{
		fprintf(stderr, "import test proto error!!!\n");
		return -1;
	}

	google::protobuf::Message* message = sProtoImporter.createDynamicMessage("lm.user2");
	if (!message)
	{
		fprintf(stderr, "create msg error!!!\n");
		return -1;
	}

	const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
	int field_count = descriptor->field_count();

	std::vector<std::string> field;
	for (int i = 0; i < field_count; i++)
	{
		std::string name = descriptor->field(i)->name();
		const google::protobuf::FieldDescriptor* fieldDescriptor = descriptor->field(i);
		bool flag = false;
		int size = static_cast<int>(tablename.size());
		for (int j = 0; j < size; j++)
		{
			if (name == tablename[j])
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			std::stringstream buf;
			buf << "add `";
			buf << name;
			buf << "` ";
			buf << pbtypetosqltype(fieldDescriptor);
			field.push_back(buf.str());
		}
	}

	if (field.size() == 0)
	{
		printf("not change\n");
		return 0;
	}
	std::stringstream sqlbuf;
	sqlbuf << "alter table user2 ";
	int size = static_cast<int>(field.size());
	for (int i = 0; i < size; i++)
	{
		sqlbuf << field[i];
		if (i != size - 1)
		{
			sqlbuf << ",";
		}
	}
	sqlbuf << ";";
	printf("sqlbuf: %s\n", sqlbuf.str().c_str());
	if (!mysql.execute(sqlbuf.str().c_str()))
	{
		fprintf(stderr, "alert error");
		return -1;
	}
	printf("alert success\n");
	return 0;
}
