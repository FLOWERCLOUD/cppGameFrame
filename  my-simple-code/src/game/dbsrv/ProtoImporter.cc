
#include "ProtoImporter.h"

#include <game/dbsrv/config/ConfigMgr.h>
#include <game/dbsrv/Util.h>

class MyMultiFileErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
        virtual void AddError(
                const std::string & filename,
                int line,
                int column,
                const std::string & message)
        {
        	fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
       }
};

static MyMultiFileErrorCollector errorCollector;
static google::protobuf::compiler::DiskSourceTree sourceTree;

ProtoImporter::ProtoImporter():
		importer(&sourceTree, &errorCollector)
{
		char* protopath = getenv("PROTO_PATH");
		if (!protopath)
		{
			sourceTree.MapPath("", "");
		}
		else
		{
			sourceTree.MapPath("", protopath);
		}
		printf("[ProtoImporter] protopath:%s\n", protopath);
		std::string filenames = sConfigMgr.MainConfig.GetStringDefault("proto", "filelist", "test.proto");

		std::vector<std::string> vec = StrSplit(filenames, ",");
		for (size_t i = 0; i < vec.size(); i++)
		{
			const  google::protobuf::FileDescriptor* filedescriptor = importer.Import(vec[i]);
			 if (!filedescriptor)
			 {
				 fprintf(stderr, "import (%s) file descriptor error\n", vec[i].c_str());
			 }
		}
}

bool ProtoImporter::Import(const std::string& filename)
{
	const  google::protobuf::FileDescriptor* filedescriptor = importer.Import(filename);
	 if (!filedescriptor)
	 {
		 fprintf(stderr, "import (%s) file descriptor error\n", filename.c_str());
		 return false;
	 }
	 return true;
}

