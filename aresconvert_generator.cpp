#include "aresconvert_generator.h"
#include "aext.pb.h"
#include "inja/inja.hpp"
#include "helpers.h"
#include "md5.h"
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
    }   else if (parameter.find(convert_opt) == 0) {
        std::string message_name = parameter.substr(convert_opt.size());
        if (!Convert(file, message_name)) return false;           
    }   else if (parameter == "struct") {
        json obj;
        obj["file_name"] = GetFileName(file->name());
        obj["package_name"] = file->package();
        if (!GenerateInfo(file, obj["code_data"])) return false;
        if (!GenerateMeta(file, obj["meta"])) return false;

        auto header_ctx = generator_context->Open(GetFileName(file->name())+ ".h");
        std::unique_ptr<io::ZeroCopyOutputStream> ptr_header_ctx(header_ctx);
        Printer header_printer(header_ctx, '$');
        if (!GenerateResourceHeader(header_printer, obj)) return false;
    }

    return true;
}

bool AResConvertGenerator::GenerateMeta(const FileDescriptor* file, json& obj) const {
    AFileMeta file_meta(file->name());
    if (!GenerateMeta(file, file_meta)) {
        return false;
    }

    std::vector<unsigned char> vec;
    file_meta.Serialize(vec);
    for (auto c: vec) {
        char hex[10];
        snprintf(hex, sizeof(hex), "%x", c);
        obj.push_back(hex);
    }
    return true;
}

bool AResConvertGenerator::GenerateResourceHeader(Printer& printer, const json& info) const {
    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    printer.PrintRaw(env.render(env.parse_template("aresconvert_resource_header.template"), info));
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

bool AResConvertGenerator::FindEnumValueInFile(const FileDescriptor* file, const std::string enum_name, int* count) const {
    auto enum_value = file->FindEnumValueByName(enum_name); 
    if (!enum_value) {
        return false;
    }

    *count = enum_value->number();
    return true;
}

bool AResConvertGenerator::FindEnumInFile(const FileDescriptor* file, const std::vector<std::string>& vec, int curr, int* count) const {
    if (curr + 1 == vec.size()) {
        return FindEnumValueInFile(file, vec[curr], count);
    }   else if (curr + 2 == vec.size()) {
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

    if (in_other_file && in_this_file) {
        ERR_RETURN(enum_name + "is ambiguous", false);
    }

    if (*count == 0) {
        ERR_RETURN(enum_name + ":size or count can not be specified 0", false);
    }
    return in_other_file || in_this_file;
}

std::string Concat(const std::string& prefix, const std::string& next) {
    if (prefix.empty()) {
        return prefix + next;
    }
    return prefix + "#" + next;
}

bool AResConvertGenerator::Flatten(const FileDescriptor* file, const Descriptor* md, ATableMeta& meta, const std::string& prefix) const {
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
                    if (!options.HasExtension(AExt::count)) {
                        ERR_RETURN(md->name() + "." + field->name() + " is repeated but does not have count", false);
                    }

                    const std::string& count  = options.GetExtension(AExt::count);
                    int num = 0;
                    if (!GetEnumValue(file, count, &num)) {
                        ERR_RETURN("Fail to find enum: " + count, false);
                    }

                    for (int i = 0; i < num; i++) {
                        std::string new_prefix = Concat(prefix, field->name() + std::to_string(i+1));
                        if (!Flatten(file, field->message_type(), meta, new_prefix)) {
                            return false;   
                        }
                    }

                }   else {
                    std::string new_prefix = Concat(prefix, field->name());
                    if (!Flatten(file, field->message_type(), meta, new_prefix)) {
                        return false;
                    }
                }

                break;
            case FieldDescriptor::TYPE_GROUP:
                ERR_RETURN("Group is not supported", false);
            default:
                if (field->is_repeated()) {
                    auto options = field->options();            
                    if (!options.HasExtension(AExt::count)) {
                        ERR_RETURN(field->name() + "is repeated but does not have count", false);
                    }

                    const std::string& count  = options.GetExtension(AExt::count);
                    int num = 0;
                    if (!GetEnumValue(file, count, &num)) {
                        ERR_RETURN("Fail to find enum: " + count, false);
                    }                    

                    for (int i = 0; i < num; i++) {
                        int type_size = GetTypeSize(field); 
                        if (type_size == 0) {
                            return false;
                        }
                        std::string field_name = Concat(prefix, field->name() + std::to_string(i+1));
                        meta.CreateField(field_name, GetFieldType(field), type_size);
                    }

                }
                else {
                    int type_size = GetTypeSize(field); 
                    if (type_size == 0) {
                        return false;
                    }
                    std::string field_name = Concat(prefix, field->name());
                    meta.CreateField(field_name, GetFieldType(field), type_size); 
                }
                break;
        }
    }
    return true;
}


bool AResConvertGenerator::Convert(const FileDescriptor* file, const std::string& message_name) const {
    auto md = file->FindMessageTypeByName(message_name);
    if (md == nullptr) {
        ERR_RETURN("Fail to find " + message_name, false);
    }

    AMessageMeta message_meta(message_name);
    if (CreateMessageMetaFromDescriptor(file, md, message_meta)) {
        GOOGLE_LOG(INFO) << "GG97\n";
        auto y = message_meta.GetMD5();
        GOOGLE_LOG(INFO) << "GG98\n";
        m_message_meta.SetMetaMD5(y);
        GOOGLE_LOG(INFO) << "GG99\n";
    }

    GOOGLE_LOG(INFO) << "GG100\n";
    if (!Flatten(file, md, m_message_meta)) {
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
        if (options.HasExtension(AExt::file_name) && options.HasExtension(AExt::xlsx_name)) {
            std::string name = options.GetExtension(AExt::file_name);
            std::string xlsx = options.GetExtension(AExt::xlsx_name);
            std::string sheet_name = options.GetExtension(AExt::sheet_name );
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

bool AResConvertGenerator::CreateMessageMetaFromDescriptor(const FileDescriptor* file, const Descriptor* descriptor, AMessageMeta& message_meta) const {
    int32_t offset = 0;
    for (int j = 0; j < descriptor->field_count(); j++) 
    {
        GOOGLE_LOG(INFO) << "GG1 " << descriptor->name() << "\n";
        const FieldDescriptor* field_descriptor = descriptor->field(j);
        int size = GetTypeSize(field_descriptor);
        if (size == 0) return false;
        if (field_descriptor->is_repeated() && !field_descriptor->options().HasExtension(AExt::count)) {
            GOOGLE_LOG(ERROR) << field_descriptor->name() + " is repeatd but does not have count";
            return false;
        }
        GOOGLE_LOG(INFO) << "GG2\n";
        int count = 1;
        if (field_descriptor->options().HasExtension(AExt::count)) {
            std::string count_str = field_descriptor->options().GetExtension(AExt::count);
            if (!GetEnumValue(file,  count_str, &count)) return false;
        }
        GOOGLE_LOG(INFO) << "GG3\n";
        AFieldMeta field_meta(field_descriptor->name(), GetStructTypeName(field_descriptor), size, offset, count);
        message_meta.CreateFieldMeta(field_meta);
        offset += count * size; 
    }
    return true;
}

bool AResConvertGenerator::GenerateMeta(const FileDescriptor* file, AFileMeta& file_meta) const {
    for (int i = 0; i < file->message_type_count(); i++)
    {
        const Descriptor* descriptor = file->message_type(i);
        AMessageMeta message_meta(descriptor->name());
        CreateMessageMetaFromDescriptor(file, descriptor, message_meta); 
        file_meta.CreateMessageMeta(message_meta);
    }

    for (int i = 0; i < file->enum_type_count(); i++)
    {
        const EnumDescriptor* descriptor = file->enum_type(i);
        AEnumMeta enum_meta(descriptor->name());
        for (int j = 0; j < descriptor->value_count(); j++)
        {
            const EnumValueDescriptor* value_descriptor = descriptor->value(j);
            enum_meta.CreateEnumFieldMeta(AEnumFieldMeta(value_descriptor->name(), value_descriptor->number()));
        }
        file_meta.CreateEnumMeta(enum_meta);
    }
    return true;
}

bool AResConvertGenerator::GenerateInfo(const FileDescriptor* file, json& info) const {
    std::string file_name = GetFileName(file->name());
    info["messages"] = json();
    for (int i = 0; i < file->message_type_count(); i++)
    {
        const Descriptor* descriptor = file->message_type(i);
        info["messages"][descriptor->name()] = json(); 
        for (int j = 0; j < descriptor->field_count(); j++) 
        {
            const FieldDescriptor* field_descriptor = descriptor->field(j);
            info["messages"][descriptor->name()][field_descriptor->name()]["tag"] = field_descriptor->number();
            info["messages"][descriptor->name()][field_descriptor->name()]["type"] = GetJsonTypeName(field_descriptor);
            info["messages"][descriptor->name()][field_descriptor->name()]["is_msg"] = field_descriptor->type() == FieldDescriptor::TYPE_MESSAGE;
            std::string field_with_type;
            if (!GetStructTypeField(field_descriptor, field_with_type)) return false ;
            info["messages"][descriptor->name()][field_descriptor->name()]["type_field"] = field_with_type;
        }

        if (descriptor->nested_type_count() > 0 || descriptor->enum_type_count() > 0) {
            GOOGLE_LOG(ERROR) << descriptor->name() <<" Error: Nested message and enum are not supported right now";
        }
    }

    info["enums"] = json();
    for (int i = 0; i < file->enum_type_count(); i++)
    {
        const EnumDescriptor* enum_descriptor = file->enum_type(i);
        info["enums"][enum_descriptor->name()] = json();
        for (int j = 0; j < enum_descriptor->value_count(); j++) 
        {
            const EnumValueDescriptor* enum_value_descriptor = enum_descriptor->value(j);
            info["enums"][enum_descriptor->name()][enum_value_descriptor->name()] = enum_value_descriptor->number();
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
            if (!options.HasExtension(AExt::size)) {
                ERR_RETURN(field->containing_type()->name() + "." + field->name() + " is string but does not have size", 0);
            }

            const std::string& enum_name= options.GetExtension(AExt::size);
            if (!GetEnumValue(field->file(), enum_name, &num)) {
                return 0; 
            }
            return num;
        }

        case FieldDescriptor::CPPTYPE_MESSAGE: {
            auto message = field->message_type();
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

FIELDTYPE AResConvertGenerator::GetFieldType(const FieldDescriptor* field) const
{
    switch (field->cpp_type()) {
        case FieldDescriptor::CPPTYPE_BOOL:
            return FIELDTYPE_UINT8;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            return FIELDTYPE_DOUBLE;
        case FieldDescriptor::CPPTYPE_FLOAT:
            return FIELDTYPE_FLOAT;
        case FieldDescriptor::CPPTYPE_INT32:
            return FIELDTYPE_INT32;
        case FieldDescriptor::CPPTYPE_INT64:
            return FIELDTYPE_INT64;
        case FieldDescriptor::CPPTYPE_UINT32:
            return FIELDTYPE_UINT32;
        case FieldDescriptor::CPPTYPE_UINT64:
            return FIELDTYPE_UINT64;
        case FieldDescriptor::CPPTYPE_ENUM:
            return FIELDTYPE_INT32;
        case FieldDescriptor::CPPTYPE_STRING:
            return FIELDTYPE_STRING;
        default: 
            return FIELDTYPE_INVALID;
    }
}

std::string AResConvertGenerator::GetStructTypeName(const FieldDescriptor* field) const
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
            return "char";
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

std::string AResConvertGenerator::GetJsonTypeName(const FieldDescriptor* field) const
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

std::string AResConvertGenerator::FindBinFileNameByMessageName(const std::string& message_name) const {
    for (auto& item: m_info.resource_list.items()) {
        if (item.value()["res_name"] == message_name) {
            return item.value()["file_name"];
        }
    }
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

bool AResConvertGenerator::GetStructTypeField(const FieldDescriptor* field, std::string& out) const{
    std::string type_name = GetStructTypeName(field);
    switch (field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
            if (field->is_map()) {
                ERR_RETURN("map is not supported", false)
            }

            if (field->is_repeated()) {
                auto options = field->options();            
                if (!options.HasExtension(AExt::count)) {
                    ERR_RETURN(field->name() + " is repeated but does not have count", false);
                }

                const std::string& count  = options.GetExtension(AExt::count);
                out = type_name + " " + field->name() + "[" + count + "]"; 

            }   else {
                out = type_name + " " + field->name();
            }
            break;
        case FieldDescriptor::TYPE_GROUP:
            ERR_RETURN("Group is not supported", false);
        case FieldDescriptor::TYPE_STRING:
            {
                auto options = field->options();            
                if (!options.HasExtension(AExt::size)) {
                    ERR_RETURN(field->containing_type()->name() + "." + field->name() + " is string but does not have size", false);
                }

                const std::string& size = options.GetExtension(AExt::size);
                if (field->is_repeated()) {
                    auto options = field->options();            
                    if (!options.HasExtension(AExt::count)) {
                        ERR_RETURN(field->name() + " is repeated but does not have count", false);
                    }

                    const std::string& count  = options.GetExtension(AExt::count);
                    out = type_name + " " + field->name() + "[" + count + "][" + size + "]"; 
                }   else {
                    out = type_name + " " + field->name() + "[" + size + "]";
                }
            }
            break;
        default:
            if (field->is_repeated()) {
                auto options = field->options();            
                if (!options.HasExtension(AExt::count)) {
                    ERR_RETURN(field->name() + "is repeated but does not have count", false);
                }

                const std::string& count  = options.GetExtension(AExt::count);
                out = type_name + " " + field->name() + "[" + count + "]"; 
            }
            else {
                out = type_name + " " + field->name();
            }
            break;
    }
    return true;
}
