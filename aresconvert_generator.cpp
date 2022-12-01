#include "aresconvert_generator.h"
#include "aextension.pb.h"
#include "inja/inja.hpp"
#include <iostream>

using namespace nlohmann;

std::string AResConvertGenerator::GetFileName(const std::string& file_name) const
{
    std::string suffix = ".proto";
    return StripSuffixString(file_name, suffix);
}

bool AResConvertGenerator::Generate(const FileDescriptor* file, 
                  const std::string& parameter,
                  GeneratorContext* generator_context, 
                  std::string* error) const
{
    json obj;
    if (!GenerateInfo(file, obj)) return false;

    if (parameter == "json") {
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
    return true;
}

bool AResConvertGenerator::FindAllResourceTable(const FileDescriptor* file) const
{
    for (int i = 0; i < file->message_type_count(); i++)
    {
        const Descriptor* descriptor = file->message_type(i);
        auto options = descriptor->options();
        if (options.HasExtension(AResConvertExt::file_name)) {
            std::string name = options.GetExtension(AResConvertExt::file_name);
            m_info.resource_list.push_back({
                {"file_name", name},
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
