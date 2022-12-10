#ifndef _ARESCONVERT_GENERATOR_H__ 
#define _ARESCONVERT_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include "nlohmann/json.hpp"
#include "AResourceHead.h"
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
    std::string FindXLSXFileNameByMessageName(const std::string& message_name) const;
    std::string FindSheetNameByMessageName(const std::string& message_name) const;
    std::string FindBinFileNameByMessageName(const std::string& message_name) const;
    const AMessageMeta& GetMetaOut() const { return m_message_meta; };

private:
    std::string GetFileName(const std::string& file_name) const;
    std::string GetJsonTypeName(const FieldDescriptor* field) const;
    bool GetStructTypeField(const FieldDescriptor* field, std::string& out) const;
    std::string GetStructTypeName(const FieldDescriptor* field) const;

    bool GenerateInfo(const FileDescriptor* file, json& info) const; 
    bool GenerateJsonHeader(Printer& printer, const json& info) const;
    bool GenerateJsonSource(Printer& printer, const json& info) const;
    bool GenerateResourceHeader(Printer& printer, const json& info) const;
    bool Convert(const FileDescriptor* file, const std::string file_name) const;
    // Generate resource info into m_info
    bool FindAllResourceTable(const FileDescriptor* file) const;

private:
    mutable DataInfo m_info;
    mutable AMessageMeta m_message_meta;

    bool Flatten(const FileDescriptor* file, const Descriptor* md, AMessageMeta& vec, const std::string& prefix="") const;
    bool GetEnumValue(const FileDescriptor* file, const std::string enum_name, int* count) const;
    bool FindEnumInFile(const FileDescriptor* file, const std::vector<std::string>& vec, int curr, int* count) const;
    bool FindEnumValueInEnumType(const EnumDescriptor* enum_type, const std::string enum_name, int* count) const;
    bool FindEnumValueInFile(const FileDescriptor* file, const std::string enum_name, int* count) const;
    bool FindEnumInMessage(const Descriptor* desc, const std::vector<std::string>& vec, int curr, int* count) const;

    int GetTypeSize(const FieldDescriptor* field) const;
    FIELDTYPE GetFieldType(const FieldDescriptor* field) const;
};

#endif // _ARESCONVERT_GENERATOR_TOTAL_H__
