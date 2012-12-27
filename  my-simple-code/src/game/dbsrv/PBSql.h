
#ifndef GAME_PBSQL_H_
#define GAME_PBSQL_H_

#include <game/dbsrv/mysql/MySQLConnection.h>

#include <game/dbsrv/codec/codec.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/importer.h>

#include <string>

class PBSql
{
public:
	static google::protobuf::Message* select(const std::string& tablename, const std::string& typeName, int uid, MySQLConnection& mysql);
	static std::string buildReplaceSql(const google::protobuf::Message* message, const std::string& tablename, MySQLConnection& mysql);
};

#endif
