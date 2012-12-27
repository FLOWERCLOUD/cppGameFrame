
#ifndef GOOGLE_PROTOBUF_COMPILER_SQL_GENERATOR_H__
#define GOOGLE_PROTOBUF_COMPILER_SQL_GENERATOR_H__

#include <string>

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/stubs/common.h>

namespace google {
namespace protobuf {

class Descriptor;
class EnumDescriptor;
class EnumValueDescriptor;
class FieldDescriptor;
class ServiceDescriptor;

namespace io { class Printer; }

namespace compiler {
namespace sql {

class Generator : public CodeGenerator {
 public:
  Generator();
  virtual ~Generator();

  // CodeGenerator methods.
  virtual bool Generate(const FileDescriptor* file,
                        const string& parameter,
                        GeneratorContext* generator_context,
                        string* error) const;

 private:

  // Very coarse-grained lock to ensure that Generate() is reentrant.
  // Guards file_, printer_ and file_descriptor_serialized_.
  mutable Mutex mutex_;
  mutable const FileDescriptor* file_;  // Set in Generate().  Under mutex_.
  mutable string file_descriptor_serialized_;
  mutable io::Printer* printer_;  // Set in Generate().  Under mutex_.

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Generator);
};

}  // namespace sql
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif
