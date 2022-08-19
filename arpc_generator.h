#ifndef ARPC_GENERATOR_H__ 
#define ARPC_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;

class ARPCGenerator: public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, 
                  const string& parameter,
                  GeneratorContext* generator_context, 
                  string* error) const;
};

#endif // ARPC_GENERATOR_TOTAL_H__
