
#include <limits>
#include <map>
#include <utility>
#include <string>
#include <vector>

#include "sql_generator.h"
#include <google/protobuf/descriptor.pb.h>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/stubs/substitute.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace sql {

// Returns a copy of |filename| with any trailing ".protodevel" or ".proto
// suffix stripped.
// TODO(robinson): Unify with copy in compiler/cpp/internal/helpers.cc.
string StripProto(const string& filename) {
  const char* suffix = HasSuffixString(filename, ".protodevel")
      ? ".protodevel" : ".proto";
  return StripSuffixString(filename, suffix);
}

// Returns the sql module name expected for a given .proto filename.
string ModuleName(const string& filename) {
  string basename = StripProto(filename);
  StripString(&basename, "-", '_');
  StripString(&basename, "/", '.');
  return basename + "_pb2";
}

Generator::Generator() : file_(NULL) {
}

Generator::~Generator() {
}

bool Generator::Generate(const FileDescriptor* file,
                         const string& parameter,
                         GeneratorContext* context,
                         string* error) const {

  MutexLock lock(&mutex_);
  file_ = file;
  string module_name = ModuleName(file->name());
  string filename = module_name;
  StripString(&filename, ".", '/');
  filename += ".sql";

  FileDescriptorProto fdp;
  file_->CopyTo(&fdp);
  fdp.SerializeToString(&file_descriptor_serialized_);


  scoped_ptr<io::ZeroCopyOutputStream> output(context->Open(filename));
  GOOGLE_CHECK(output.get());
  io::Printer printer(output.get(), '$');
  printer_ = &printer;

  int msg_type_count = file_->message_type_count();
  for (int i = 0; i < msg_type_count; i++)
  {
	  const Descriptor* descriptor = file_->message_type(i);
	  int field_count = descriptor->field_count();
	  for (int j = 0; j < field_count; j++)
	  {
		  const FieldDescriptor* fieldDes = descriptor->field(j);
		  printf("%d: %s: %s\n", fieldDes->type(), fieldDes->name().c_str(), fieldDes->options().DebugString().c_str());
	  }
	  //printf("%s\n", descriptor->DebugString().c_str());
  }
  printf("hello world, filename: %s\n\n", filename.c_str());

  printer.Print(
    "# hello world\n");

  return !printer.failed();
}

}  // namespace sql
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
