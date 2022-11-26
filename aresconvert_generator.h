#ifndef _ARESCONVERT_GENERATOR_H__ 
#define _ARESCONVERT_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;
using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::internal;
using namespace google::protobuf::io;

class AResConvertGenerator: public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, 
                  const string& parameter,
                  GeneratorContext* generator_context, 
                  string* error) const;
                  

private:
    std::string GetFileName(const std::string& file_name) const;
    std::string GetTypeName(const FieldDescriptor* field) const;

    bool GenerateInfo(const FileDescriptor* file, json& info) const; 
    bool GenerateHeader(Printer& printer, const json& info) const;
    bool GenerateSource(Printer& printer, const json& info) const;

};

#endif // _ARESCONVERT_GENERATOR_TOTAL_H__
