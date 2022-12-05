#include "aresconvert_generator.h"
#include "aextension.pb.h"
#include "inja/inja.hpp"
#include "helpers.h"
#include <iostream>

using namespace nlohmann;

std::string AResConvertGenerator::GetFileName(const std::string& file_name) const
{
    std::string suffix = ".proto";
    return StripSuffixString(file_name, suffix);
}

#define ERR_RETURN(message__, ret__)\
    GOOGLE_LOG(ERROR)  << message__;\
    return ret__;

bool AResConvertGenerator::Generate(const FileDescriptor* file, 
                  const std::string& parameter,
                  GeneratorContext* generator_context, 
                  std::string* error) const
{
    std::string convert_opt = "convert:";
    if (parameter == "json") {
        json obj;
        if (!GenerateInfo(file, obj)) return false;
        auto header_ctx = generator_context->Open(GetFileName(file->name())+ ".h");
        std::unique_ptr<io::ZeroCopyOutputStream> ptr_header_ctx(header_ctx);
        Printer header_printer(header_ctx, '$');
        if (!GenerateJsonHeader(header_printer, obj)) return false;

        auto source_ctx = generator_context->Open(GetFileName(file->name())+ ".cpp");
        std::unique_ptr<io::ZeroCopyOutputStream> ptr_sourc_ctx(source_ctx);
        Printer source_printer(source_ctx, '$');
        if (!GenerateJsonSource(source_printer, obj)) return false;
    }
    else if (parameter == "resource") {
        if (!FindAllResourceTable(file)) return false ;           
    }
    else if (parameter.find(convert_opt) == 0) {
        std::string file_name = parameter.substr(convert_opt.size());
        if (!Convert(file, file_name)) return false ;           
    }
    return true;
}

bool AResConvertGenerator::FindEnumInMessage(const Descriptor* desc, const std::vector<std::string>& vec, int curr, int* count) const {
    if (curr + 2 == vec.size()) {
        auto enum_type = desc->FindEnumTypeByName(vec[curr]);
        if (!enum_type) {
            return false;
        }
        return FindEnumValueInEnumType(enum_type, vec[curr+1], count);
    }

    if (curr < 0 || curr >= vec.size()) {
        return false;
    }
    auto next = desc->FindNestedTypeByName(vec[curr]);
    if (next) {
        return FindEnumInMessage(next, vec, curr+1, count);
    }
    return false;
}

bool AResConvertGenerator::FindEnumValueInEnumType(const EnumDescriptor* enum_type, const std::string enum_name, int* count) const {
    auto enum_value = enum_type->FindValueByName(enum_name); 
    if (!enum_value) {
        return false;
    }

    *count = enum_value->number();
    return true;
}

bool AResConvertGenerator::FindEnumInFile(const FileDescriptor* file, const std::vector<std::string>& vec, int curr, int* count) const {
    if (curr + 2 == vec.size()) {
        auto enum_type = file->FindEnumTypeByName(vec[curr]);
        if (!enum_type) {
            return false;
        }
        return FindEnumValueInEnumType(enum_type, vec[curr+1], count);
    }

    if (curr < 0 || curr >= vec.size()) {
        return false;
    }
    auto next = file->FindMessageTypeByName(vec[curr]);
    if (next){
       return FindEnumInMessage(next, vec, curr+1, count); 
    }
    return false;
}

bool AResConvertGenerator::GetEnumValue(const FileDescriptor* file, const std::string enum_name, int* count) const {
    bool in_other_file = false;
    bool in_this_file = false;
    std::vector<std::string> strs = stringSplit(enum_name, "::");
    for (int i = 0; i < file->dependency_count(); ++i) {
        const FileDescriptor* dep_file = file->dependency(i);
        if (dep_file->package() == strs[0] && FindEnumInFile(dep_file, strs, 1, count)) {
            in_other_file = true;
            break;
        }
    }

    in_this_file = FindEnumInFile(file, strs, 0, count);

    if (*count == 0) {
        ERR_RETURN(enum_name + ":size or count can not be specified 0", false);
    }

    if (in_other_file && in_this_file) {
        ERR_RETURN(enum_name + "is ambiguous", false);
    }
    return in_other_file || in_this_file;
}

std::string Concat(const std::string& prefix, const std::string& next) {
    if (prefix.empty()) {
        return prefix + next;
    }
    return prefix + "#" + next;
}

bool AResConvertGenerator::Flatten(const FileDescriptor* file, const Descriptor* md, std::vector<FieldMeta>& vec, const std::string& prefix) const {
    for (int j = 0; j < md->field_count(); j++) 
    {
        auto field = md->field(j);
        switch (field->type()) {
            case FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                    ERR_RETURN("map is not supported", false)
                    return false;
                }

                if (field->is_repeated()) {
                    auto options = field->options();            
                    if (!options.HasExtension(AResConvertExt::count)) {
                        ERR_RETURN(md->name() + "." + field->name() + " is repeated but does not have count", false);
                    }

                    const std::string& count  = options.GetExtension(AResConvertExt::count);
                    int num = 0;
                    if (!GetEnumValue(file, count, &num)) {
                        ERR_RETURN("Fail to find enum: " + count, false);
                    }

                    for (int i = 0; i < num; i++) {
                        std::string new_prefix = Concat(prefix, field->name() + std::to_string(i+1));
                        Flatten(file, field->message_type(), vec, new_prefix);
                    }

                }
                else {
                    std::string new_prefix = Concat(prefix, field->name());
                    Flatten(file, field->message_type(), vec, new_prefix);
                }

                break;
            case FieldDescriptor::TYPE_GROUP:
                ERR_RETURN("Group is not supported", false);
            default:
                if (field->is_repeated()) {
                    auto options = field->options();            
                    if (!options.HasExtension(AResConvertExt::count)) {
                        ERR_RETURN(field->name() + "is repeated but does not have count", false);
                    }

                    const std::string& count  = options.GetExtension(AResConvertExt::count);
                    int num = 0;
                    if (!GetEnumValue(file, count, &num)) {
                        ERR_RETURN("Fail to find enum: " + count, false);
                    }                    

                    for (int i = 0; i < num; i++) {
                        vec.emplace_back(Concat(prefix, field->name() + std::to_string(i+1)), GetTypeSize(field));
                    }

                }
                else {
                    vec.emplace_back(Concat(prefix, field->name()), GetTypeSize(field)); 
                }
                break;
        }
    }
    return true;
}

bool AResConvertGenerator::Convert(const FileDescriptor* file, const std::string message_name) const {
    auto md = file->FindMessageTypeByName(message_name);
    if (md == nullptr) {
        ERR_RETURN("Fail to find " + message_name, false);
    }
    if (!Flatten(file, md, m_cols)) {
        return false;
    }
    return true;
}

bool AResConvertGenerator::FindAllResourceTable(const FileDescriptor* file) const
{
    m_info.resource_list.clear();
    for (int i = 0; i < file->message_type_count(); i++)
    {
        const Descriptor* descriptor = file->message_type(i);
        auto options = descriptor->options();
        if (options.HasExtension(AResConvertExt::file_name) && options.HasExtension(AResConvertExt::xlsx_name)) {
            std::string name = options.GetExtension(AResConvertExt::file_name);
            std::string xlsx = options.GetExtension(AResConvertExt::xlsx_name);
            std::string sheet_name = options.GetExtension(AResConvertExt::sheet_name );
            m_info.resource_list.push_back({
                {"file_name", name},
                {"xlsx_name", xlsx}, 
                {"sheet_name", sheet_name},
                {"res_name", descriptor->name()}
            });
        }
    }

    return true;   
}

bool AResConvertGenerator::GenerateJsonHeader(Printer& printer, const json& info) const {
    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    printer.PrintRaw(env.render(env.parse_template("aresconvert_header.template"), info));
    return true;
}

bool AResConvertGenerator::GenerateJsonSource(Printer& printer, const json& info) const {
    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    printer.PrintRaw(env.render(env.parse_template("aresconvert_source.template"), info));
    return true;
}

bool AResConvertGenerator::GenerateInfo(const FileDescriptor* file, json& info) const {
    std::string file_name = GetFileName(file->name());
    info["messages"] = json();
    info["file_name"] = file_name;
    for (int i = 0; i < file->message_type_count(); i++)
    {
        const Descriptor* descriptor = file->message_type(i);
        info["messages"][descriptor->name()] = json(); 
        for (int j = 0; j < descriptor->field_count(); j++) 
        {
            const FieldDescriptor* field_descriptor = descriptor->field(j);
            info["messages"][descriptor->name()][field_descriptor->name()]["tag"] = field_descriptor->number();
            info["messages"][descriptor->name()][field_descriptor->name()]["type"] = GetTypeName(field_descriptor);
            info["messages"][descriptor->name()][field_descriptor->name()]["is_msg"] = field_descriptor->type() == FieldDescriptor::TYPE_MESSAGE;
        }
    }

    return true;
}

int AResConvertGenerator::GetTypeSize(const FieldDescriptor* field) const {
    switch (field->cpp_type()) {
        case FieldDescriptor::CPPTYPE_BOOL:
            return 1;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            return 8;
        case FieldDescriptor::CPPTYPE_FLOAT:
            return 4;
        case FieldDescriptor::CPPTYPE_INT32:
            return 4;
        case FieldDescriptor::CPPTYPE_INT64:
            return 8;
        case FieldDescriptor::CPPTYPE_UINT32:
            return 4;
        case FieldDescriptor::CPPTYPE_UINT64:
            return 8;
        case FieldDescriptor::CPPTYPE_ENUM:
            return 4;
        case FieldDescriptor::CPPTYPE_STRING: {
            int num = 0;
            auto options = field->options();            
            if (!options.HasExtension(AResConvertExt::size)) {
                ERR_RETURN(field->containing_type()->name() + "." + field->name() + " is repeated but does not have count", 0);
            }

            const std::string& enum_name= options.GetExtension(AResConvertExt::size);
            if (!GetEnumValue(field->file(), enum_name, &num)) {
                return 0; 
            }
            return num;
        }

        case FieldDescriptor::CPPTYPE_MESSAGE: {
            auto message = field->containing_type();
            int total = 0;
            for (int i = 0; i < message->field_count(); ++i) {
               total += GetTypeSize(message->field(i));
            }
            return total;
        }
    }
    GOOGLE_LOG(ERROR) << "Fail to find type size for";
    return 0;
}

std::string AResConvertGenerator::GetTypeName(const FieldDescriptor* field) const
{
    switch (field->type()) {
        case FieldDescriptor::TYPE_DOUBLE:
            return "double";
        case FieldDescriptor::TYPE_FLOAT:
            return "float";
        case FieldDescriptor::TYPE_INT64:
            return "int64_t";
        case FieldDescriptor::TYPE_UINT64:
            return "uint64_t";
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_INT32:
            return "int32_t";
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_FIXED64:
            return "int64_t";
        case FieldDescriptor::TYPE_FIXED32:
            return "int32_t";
        case FieldDescriptor::TYPE_BOOL:
            return "bool";
        case FieldDescriptor::TYPE_STRING:
            return "std::string";
        case FieldDescriptor::TYPE_GROUP:
            assert(field->type() != FieldDescriptor::TYPE_GROUP && "TYPE_GROUP is not allowed");
            return "";
        case FieldDescriptor::TYPE_MESSAGE:
            return field->message_type()->name();
        case FieldDescriptor::TYPE_BYTES:
            return "uint8_t";
        case FieldDescriptor::TYPE_UINT32:
            return "uint32_t";
        case FieldDescriptor::TYPE_ENUM:
            return field->type_name();
    }
    assert(false && "Invalid type given");
    return "";
}

std::string AResConvertGenerator::FindXLSXFileNameByMessageName(const std::string& message_name) const {
    for (auto& item: m_info.resource_list.items()) {
        if (item.value()["res_name"] == message_name) {
            return item.value()["xlsx_name"];
        }
    }
    return "";
}

std::string AResConvertGenerator::FindSheetNameByMessageName(const std::string& message_name) const {
    for (auto& item: m_info.resource_list.items()) {
        if (item.value()["res_name"] == message_name) {
            return item.value()["sheet_name"];
        }
    }
    return "";
}
