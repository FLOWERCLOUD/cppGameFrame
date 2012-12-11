
#ifndef GAME_PROTOIMPORTER_H_
#define GAME_PROTOIMPORTER_H_

#include <mysdk/base/Singleton.h>

#include <string>

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

class ProtoImporter
{
public:
	ProtoImporter();
public:
	bool Import(const std::string& filename);

public:
	google::protobuf::compiler::Importer importer;
	google::protobuf::DynamicMessageFactory factory;
};

#define sProtoImporter mysdk::Singleton<ProtoImporter>::instance()

#endif
