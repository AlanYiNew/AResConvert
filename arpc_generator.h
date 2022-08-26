#ifndef ARPC_GENERATOR_H__ 
#define ARPC_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::internal;
using namespace google::protobuf::io;

class ARPCGenerator: public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, 
                  const string& parameter,
                  GeneratorContext* generator_context, 
                  string* error) const;
                  
    bool GenerateDefineFile(const FileDescriptor* file, Printer& printer) const; 
};

#endif // ARPC_GENERATOR_TOTAL_H__
