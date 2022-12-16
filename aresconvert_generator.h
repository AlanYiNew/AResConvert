#ifndef _ARESCONVERT_GENERATOR_H__ 
#define _ARESCONVERT_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include "nlohmann/json.hpp"
#include "AMeta.h"
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

struct ATableFieldMeta {
    std::string name;
    int32_t size;
    int32_t offset;
    FIELDTYPE field_type;
    ATableFieldMeta() =  default;
    ATableFieldMeta(const std::string& field, FIELDTYPE field_type, int32_t size, int32_t offset): field_type(field_type), name(field), size(size), offset(offset) {};
};

struct ATableMeta {
    int32_t m_size{}; // size of bytes of the records
    std::map<std::string, ATableFieldMeta> meta_data;
    std::string md5;
    void CreateField(const std::string& name, FIELDTYPE field_type, int32_t size) {
        meta_data.emplace(name, ATableFieldMeta{name, field_type, size, m_size});
        m_size += size;
    };
    const ATableFieldMeta* GetFieldMeta(const std::string& name) const {
        auto iter = meta_data.find(name);
        if (iter != meta_data.end()) {
            return &iter->second;
        }
        return nullptr;
    }

    const std::map<std::string, ATableFieldMeta>& GetFieldMetas() const {
        return meta_data;
    }

    int32_t GetSize() { return m_size; };

    void SetMetaMD5(const std::string& md5) {
        this->md5 = md5;
    }

    std::string GetMetaMD5() {
        return md5; 
    }
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
    const ATableMeta& GetMetaOut() const { return m_message_meta; };

private:
    std::string GetFileName(const std::string& file_name) const;
    std::string GetJsonTypeName(const FieldDescriptor* field) const;
    bool GetStructTypeField(const FieldDescriptor* field, std::string& out) const;
    std::string GetStructTypeName(const FieldDescriptor* field) const;

    bool GenerateInfo(const FileDescriptor* file, json& info) const; 
    bool GenerateMeta(const FileDescriptor* file, AFileMeta& file_meta) const;
    bool GenerateMeta(const FileDescriptor* file, json& obj) const;
    bool GenerateJsonHeader(Printer& printer, const json& info) const;
    bool GenerateJsonSource(Printer& printer, const json& info) const;
    bool GenerateResourceHeader(Printer& printer, const json& info) const;

    bool CreateMeta(const FileDescriptor* file) const;
    bool CreateMessageMetaFromDescriptor(const FileDescriptor* file, const Descriptor* descriptor, AMessageMeta& message_meta) const;
    bool Convert(const FileDescriptor* file, const std::string& file_name) const;
    bool FindAllResourceTable(const FileDescriptor* file) const;

private:
    mutable DataInfo m_info;
    mutable ATableMeta m_message_meta;

    bool Flatten(const FileDescriptor* file, const Descriptor* md, ATableMeta& vec, const std::string& prefix="") const;
    bool GetEnumValue(const FileDescriptor* file, const std::string enum_name, int* count) const;
    bool FindEnumInFile(const FileDescriptor* file, const std::vector<std::string>& vec, int curr, int* count) const;
    bool FindEnumValueInEnumType(const EnumDescriptor* enum_type, const std::string enum_name, int* count) const;
    bool FindEnumValueInFile(const FileDescriptor* file, const std::string enum_name, int* count) const;
    bool FindEnumInMessage(const Descriptor* desc, const std::vector<std::string>& vec, int curr, int* count) const;

    int GetTypeSize(const FieldDescriptor* field) const;
    FIELDTYPE GetFieldType(const FieldDescriptor* field) const;
};

#endif // _ARESCONVERT_GENERATOR_TOTAL_H__
