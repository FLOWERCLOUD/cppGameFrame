
#include "PBSql.h"

#include <sstream>

static std::string buildSelectSql(const google::protobuf::Descriptor* descriptor, const std::string& tablename, int uid)
{
	if(!descriptor) return "";

	std::stringstream buf;
	buf << "select ";
	int field_count = descriptor->field_count();
	for (int i = 0; i < field_count; i++)
	{
		buf << descriptor->field(i)->name();

		if (i != field_count - 1)
		{
			buf << ", ";
		}
	}
	buf << " from ";
	buf << tablename;
	buf << " where uid = ";
	buf << uid;
	return buf.str();
}

google::protobuf::Message* PBSql::select(const std::string& tablename, const std::string& typeName, int uid, MySQLConnection& mysql)
{
	google::protobuf::Message* message = KabuCodec::createDynamicMessage(typeName);
	if (!message)
	{
		return NULL;
	}
	const google::protobuf::Reflection* reflection = message->GetReflection();
	const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
	std::string query_sql = buildSelectSql(descriptor, tablename, uid);
	//printf("sql: %s\n", query_sql.c_str());
    MYSQL_RES *result = NULL;
    MYSQL_FIELD *fields = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;
    bool ret = mysql.query(query_sql.c_str(), &result, &fields, &rowCount, &fieldCount);
    if (!ret || !result)
    {
    	delete message;
    	return NULL;
    }

    for (uint32 i = 0; i < fieldCount; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row) continue;

    	const google::protobuf::FieldDescriptor *field = descriptor->FindFieldByName(fields[i].name);
    	if (!field) continue;
    	if (field->is_repeated())
    	{
    		fprintf(stderr, "current not support for repeated label\n");
    		delete message;
    		mysql_free_result(result);
    		return NULL;
    	}

    	if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
    	{
    		ulong* fieldLength = mysql_fetch_lengths(result);
    	    reflection->SetString(message, field, std::string(row[i], static_cast<int>(*fieldLength)));
    	}
    	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
    	{
    	     int val = static_cast<int>(atol(row[i]));
    	     reflection->SetInt32(message, field, val);
    	}
    	else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
    	{
    	      unsigned int val = static_cast<unsigned int>(atol(row[i]));
    	      reflection->SetUInt32(message, field, val);
    	 }
    	 else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
    	 {
    	     float val = static_cast<float>(atol(row[i]));
    	     reflection->SetFloat(message, field, val);
    	 }
    	 else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
    	 {
    	      double val = static_cast<double>(atol(row[i]));
    	      reflection->SetDouble(message, field, val);
    	 }
    	 else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
    	 {
    	      bool val = static_cast<bool>(atol(row[i]));
    	      reflection->SetBool(message, field, val);
    	  }
    	 else if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
    	 {
    		 ulong* fieldLength = mysql_fetch_lengths(result);
    		 google::protobuf::Message* msg = reflection->MutableMessage(message, field);
    		 msg->ParsePartialFromArray(row[i], static_cast<int>(*fieldLength));
    	 }
    	 else
    	 {
    		 fprintf(stderr, "current not support for TYPE\n");
    		 delete message;
    		 mysql_free_result(result);
    		 return NULL;
    	 }
    }

    mysql_free_result(result);
	return message;
}


std::string PBSql::buildReplaceSql(const google::protobuf::Message* message, const std::string& tablename, MySQLConnection& mysql)
{
	if (!message) return "";

	const google::protobuf::Reflection* reflection = message->GetReflection();

	std::stringstream buf;
	buf << "replace into ";
	buf << tablename;
	buf << "(";

	std::vector<const google::protobuf::FieldDescriptor*> output;
	reflection->ListFields(*message, &output);
	int size = static_cast<int>(output.size());
	for (int i = 0; i < size; i++)
	{
		buf << output[i]->name();

		if (i != size - 1)
		{
			buf << ",";
		}
	}

	buf << ")values(";

	for (int i = 0; i < size; i++)
	{
		const google::protobuf::FieldDescriptor* field = output[i];
		if (field->is_repeated())
		{
			fprintf(stderr, "current not support for repeated label\n");
			continue;
		}
		if(field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
		{
			//std::string str = reflection->GetString(*message, field);
			std::string str;
			str = reflection->GetStringReference(*message, field, &str);
			char str_rs[1024 * 10]; // 10k空间
			unsigned long len = mysql.format_to_real_string(str_rs, str.c_str(), str.length());
			if (len == 0) return "";

			buf << "'";
			buf << str_rs;
			buf << "'";
		}
		else if(field->type() == google::protobuf::FieldDescriptor::TYPE_INT32)
		{
			buf << reflection->GetInt32(*message, field);
		}
		else if(field->type() == google::protobuf::FieldDescriptor::TYPE_UINT32)
		{
			buf << reflection->GetUInt32(*message, field);
		}
		else if(field->type() == google::protobuf::FieldDescriptor::TYPE_FLOAT)
		{
			buf << reflection->GetFloat(*message, field);
		}
		else if(field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE)
		{
			buf << reflection->GetDouble(*message, field);
		}
		else if(field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL)
		{
			buf << reflection->GetBool(*message, field);
		}
		else if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			const google::protobuf::Message& msg = reflection->GetMessage(*message, field);

			std::string str;
			msg.SerializeToString(&str);
			//std::string str(msg.SerializeAsString());
			char str_rs[1024 * 20]; // 20k空间
			unsigned long len = mysql.format_to_real_string(str_rs, str.c_str(), str.length());
			if (len == 0) return "";

			buf << "'";
			buf << str_rs;
			buf << "'";
		}
		else
		{
			 fprintf(stderr, "current not support type\n");
			 return "";
		}

		if (i != size - 1)
		{
			buf << ", ";
		}
	}
	buf << ")";
	return buf.str();
}
