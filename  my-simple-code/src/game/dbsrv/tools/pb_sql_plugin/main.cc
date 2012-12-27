
#include <google/protobuf/compiler/command_line_interface.h>
#include "sql_generator.h"

int main(int argc, char* argv[]) {

  google::protobuf::compiler::CommandLineInterface cli;
  cli.AllowPlugins("protoc-");

  // Proto2 sql
  google::protobuf::compiler::sql::Generator sql_generator;
  cli.RegisterGenerator("--sql_out", &sql_generator,
                        "Generate sql source file.");

  return cli.Run(argc, argv);
}
