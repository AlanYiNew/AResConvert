#ifndef _ARESCONVERT_GENERATOR_H__ 
#define _ARESCONVERT_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include "nlohmann/json.hpp"
#include <vector>

using namespace nlohmann;
using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::internal;
using namespace google::protobuf::io;


struct ResourceTable {
    std::string file;
    std::string message_type_name;
};

struct DataInfo {
    json resource_list;
};


class AResConvertGenerator: public CodeGenerator {
public:
    bool Generate(const FileDescriptor* file, 
                  const std::string& parameter,
                  GeneratorContext* generator_context, 
                  std::string* error) const;
    
    const json& GetResourceNameAll() const { return m_info.resource_list; };

private:
    std::string GetFileName(const std::string& file_name) const;
    std::string GetTypeName(const FieldDescriptor* field) const;

    bool GenerateInfo(const FileDescriptor* file, json& info) const; 
    bool GenerateJsonHeader(Printer& printer, const json& info) const;
    bool GenerateJsonSource(Printer& printer, const json& info) const;
    // Generate resource info into m_info
    bool FindAllResourceTable(const FileDescriptor* file) const;

private:
    mutable DataInfo m_info;

};

#endif // _ARESCONVERT_GENERATOR_TOTAL_H__
