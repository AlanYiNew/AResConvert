#include <cstddef>
#include <filesystem>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include "application.h"
#include "custom_endian.h"
#include "md5.h"
#include "helpers.h"
#include <ctime>
#include <thread>
#include <future>

inline int32_t Application::WriteInt32(std::FILE* f, int32_t val) {
    if (!m_is_little_endian) {
        val = htole32(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteUInt32(std::FILE* f, uint32_t val) {
    if (!m_is_little_endian) {
        val = htole32(val);
    }
    std::fwrite(&val, sizeof(val), 1 , f);
    return sizeof(val);
}

inline int32_t Application::WriteInt16(std::FILE* f, int16_t val) {
    if (!m_is_little_endian) {
        val = htole16(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteUInt16(std::FILE* f, uint16_t val) {
    if (!m_is_little_endian) {
        val = htole16(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteInt64(std::FILE* f, int64_t val) {
    if (!m_is_little_endian) {
        val = htole64(val);
    }
    std::fwrite(&val, sizeof(val), 1,  f);
    return sizeof(val);
}

inline int32_t Application::WriteUInt64(std::FILE* f, uint64_t val) {
    if (!m_is_little_endian) {
        val = htole64(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteInt8(std::FILE* f, int8_t val) {
    std::fwrite(&val, sizeof(val), 1,f);
    return sizeof(val);
}

inline int32_t Application::WriteUInt8(std::FILE* f, uint8_t val) {
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteDouble(std::FILE* f, double val) {
    if (!m_is_little_endian) {
        val = htole64(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteFloat(std::FILE* f, float val) {
    if (!m_is_little_endian) {
        val = htole32(val);
    }
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t Application::WriteString(std::FILE* f, const std::string& val) {
    std::fwrite(val.c_str(), 1, val.size() + 1, f);
    return val.size() + 1;
}

inline int32_t Application::WriteBytes(std::FILE* f, const std::string& val) {
    std::fwrite(val.c_str(), 1, val.size(), f);
    return val.size();
}

inline int32_t Application::WriteBytes(std::FILE* f, char* buffer, int32_t size) {
    std::fwrite(buffer, 1, size, f);
    return size;
}

inline int32_t Application::WriteUInt32(void* f, uint32_t val) {
    if (!m_is_little_endian) {
        (*(uint32_t*)f) = htole32(val);
    }   else {
        (*(uint32_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteInt32(void* f, int32_t val) {
    if (!m_is_little_endian) {
        (*(int32_t*)f) = htole32(val);
    }   else {
        (*(int32_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteInt16(void* f, int16_t val) {
    if (!m_is_little_endian) {
        (*(int16_t*)f) = htole16(val);
    }   else {
        (*(int16_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteUInt16(void* f, uint16_t val) {
    if (!m_is_little_endian) {
        (*(uint16_t*)f) = htole16(val);
    }   else {
        (*(uint16_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteInt64(void* f, int64_t val) {
    if (!m_is_little_endian) {
        (*(int64_t*)f) = htole64(val);
    }   else {
        (*(int64_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteUInt64(void* f, uint64_t val) {
    if (!m_is_little_endian) {
        (*(uint64_t*)f) = htole64(val);
    }   else {
        (*(uint64_t*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteInt8(void* f, int8_t val) {
    (*(int8_t*)f) = val;
    return sizeof(val);
}

inline int32_t Application::WriteUInt8(void* f, uint8_t val) {
    (*(uint8_t*)f) = val;
    return sizeof(val);
}

inline int32_t Application::WriteDouble(void* f, double val) {
    if (!m_is_little_endian) {
        (*(double*)f) = htole64(val);
    }   else {
        (*(double*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteFloat(void* f, float val) {
    if (!m_is_little_endian) {
        (*(float*)f) = htole32(val);
    }   else {
        (*(float*)f) = val;
    }
    return sizeof(val);
}

inline int32_t Application::WriteString(void* f, const std::string& val) {
    strcpy((char*)f, val.c_str());
    return val.size() + 1;
}

#ifdef _WIN32
bool Application::WriteInt32Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    int32_t val;
    if  (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<int32_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else if (cell.type() != OpenXLSX::XLValueType::Integer) {
        return false;
    }
    WriteInt32(record_buffer + field_meta.offset, val);
    return true;
}

bool Application::WriteUInt32Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    uint32_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<uint32_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteUInt32(record_buffer + field_meta.offset, val);
    return true;
}

bool Application::WriteInt16Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    int16_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<int16_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0; 
    }   else {
        return false;
    }
    WriteInt16(record_buffer + field_meta.offset, val);
    return true;
}

bool Application::WriteUInt16Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    uint16_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<uint16_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteUInt16(record_buffer + field_meta.offset, val);
    return true;
}

bool Application::WriteInt64Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    int64_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<int64_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0; 
    }   else {
        return false;
    }
    WriteInt64(record_buffer + field_meta.offset, val);
    return true;
}

inline bool Application::WriteUInt64Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    uint64_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<uint64_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0; 
    }   else {
        return false;
    }
    WriteUInt64(record_buffer + field_meta.offset, val);
    return true;
}

inline bool Application::WriteInt8Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    int8_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<int8_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteInt8(record_buffer + field_meta.offset, val);
    return true;
}

inline bool Application::WriteUInt8Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    uint8_t val;
    if (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = cell.get<uint8_t>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteUInt8(record_buffer + field_meta.offset, val);
    return true;
}


inline bool Application::WriteDoubleCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    double val;
    if (cell.type() == OpenXLSX::XLValueType::Float) {
        val = cell.get<double>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteDouble(record_buffer + field_meta.offset, val);
    return true;
}

inline bool Application::WriteFloatCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    float val;
    if (cell.type() == OpenXLSX::XLValueType::Float) {
        val = cell.get<float>();
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = 0;
    }   else {
        return false;
    }
    WriteFloat(record_buffer + field_meta.offset, val);
    return true;
}

inline bool Application::WriteStringCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    std::string val;
    if (cell.type() == OpenXLSX::XLValueType::String) {
        val = cell.get<std::string>();
        if (val.size() >= field_meta.size) {
            GOOGLE_LOG(ERROR) << "string: " << val << " is greater equal to " << field_meta.size;
            return false;
        }
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
    }   else {
        return false;
    }
    WriteString(record_buffer + field_meta.offset, val);
    return true;
}
#endif

bool Application::SerializeToBin(const std::string& res_name) {
#ifdef _WIN32
    std::string xlsx_name = aresconvert_generator->FindXLSXFileNameByMessageName(res_name);
    std::string sheet_name = aresconvert_generator->FindSheetNameByMessageName(res_name);
    std::string bin_name = aresconvert_generator->FindBinFileNameByMessageName(res_name);

    time_t start = std::time(nullptr);
    OpenXLSX::XLDocument doc;
    doc.open(conf.xlsx_folder + "/" + xlsx_name);
    auto wks = doc.workbook().worksheet(sheet_name);
    int first_row = 1;
    if (wks.cell("A1").value() == "##NOTE##") {
        first_row = 2;
    }

    if (!std::filesystem::exists(conf.output_folder)) {
        std::filesystem::create_directories(conf.output_folder);
    }   else if (!std::filesystem::is_directory(conf.output_folder)) {
        GOOGLE_LOG(ERROR) << conf.output_folder << "already exists but is not a directory";
        return false;
    }

    std::string out_file = conf.output_folder + "/" + bin_name;
    std::FILE* f = std::fopen(out_file.c_str(), "wb+");
    if (f == nullptr) {
        GOOGLE_LOG(ERROR) << "fail to open " << out_file;
        return false;
    }

    int row_count = wks.rowCount();
    int col_count = wks.columnCount();

    auto message_meta = aresconvert_generator->GetMetaOut();
    std::map<std::string, AFieldMeta> field_metas = message_meta.GetFieldMetas();
    std::vector<ColData> columns;
    std::unordered_set<std::string> col_names;

    const std::vector<OpenXLSX::XLCellValue>& values = wks.row(first_row).values();
    for (const auto& cell: values) {
        int size = columns.size();
        std::string name = cell.get<std::string>();
        auto field_meta = message_meta.GetFieldMeta(name);

        if (col_names.find(name) != col_names.end()) {
            GOOGLE_LOG(ERROR) << "Duplicate column name: " << name;
            return false;
        }
        col_names.insert(name);

        if (field_meta != nullptr) {
            columns.emplace_back(*field_meta, true);
            field_metas.erase(name);
        }   else {
            columns.emplace_back(false);
        }
    }


    if (field_metas.size() != 0) {
        for (const auto& field_meta: field_metas) {
            GOOGLE_LOG(ERROR) << field_meta.first << " can not be found"; 
        }
        return false;
    }

    AResourceHead header;
    // 1. record size
    header.size = message_meta.GetSize();
    std::fseek(f, offsetof(AResourceHead, size), SEEK_SET);
    WriteInt32(f, header.size);
    // 2. md5
    std::fseek(f, offsetof(AResourceHead, meta_md5), SEEK_SET);
    WriteBytes(f, message_meta.GetMD5());
    // 3. record count
    header.count =  row_count - first_row;
    std::fseek(f, offsetof(AResourceHead, count), SEEK_SET);
    WriteInt32(f, header.count);
    // 4. record total size
    header.total_size = header.count * header.size;
    std::fseek(f, offsetof(AResourceHead, total_size), SEEK_SET);
    WriteInt32(f, header.total_size);

    char* buffer = new char[header.count * header.size];
    int rowNo = 0; 
    for (const auto& rowData : wks.rows()) {
        rowNo++;
        if (rowNo <= first_row) {
            continue;
        }

        char* offset = buffer + (rowNo - first_row - 1) * header.size;
        const std::vector<OpenXLSX::XLCellValue>& values = rowData.values();
        for (int col = 0; col < values.size(); col++) {
            const AFieldMeta& field_meta = columns[col].GetMeta();
            if (!columns[col].IsValid()) {
                continue;
            }

            const auto& cell = values[col];
            bool result = true;
            const auto field_type = field_meta.field_type;
            switch (field_type) {
                case FIELDTYPE_INT32: { result = WriteInt32Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_UINT32: { result = WriteUInt32Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_INT16: { result = WriteInt16Cell(offset, field_meta, cell); } break;   
                case FIELDTYPE_UINT16: { result = WriteUInt16Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_INT64: { result = WriteInt64Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_UINT64: { result = WriteUInt64Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_INT8: { result = WriteInt8Cell(offset, field_meta, cell); } break;   
                case FIELDTYPE_UINT8: { result = WriteUInt8Cell(offset, field_meta, cell); } break;
                case FIELDTYPE_FLOAT: { result = WriteFloatCell(offset, field_meta, cell); } break;
                case FIELDTYPE_DOUBLE: { result = WriteDoubleCell(offset, field_meta, cell); } break;
                case FIELDTYPE_STRING: { result = WriteStringCell(offset, field_meta, cell); } break;
                default: {
                    GOOGLE_LOG(ERROR) << "Invalid field_type: " << field_type;
                }
            }
            if (!result) {
                GOOGLE_LOG(ERROR) << GetCellNo(rowNo, col);
            }
        }
    }

    // 5. content md5
    m_md5 = std::make_unique<MD5>(); 
    m_md5->update(buffer, header.count * header.size);
    std::string md5 = m_md5->toString();
    std::fseek(f, offsetof(AResourceHead, content_md5), SEEK_SET);
    WriteBytes(f, md5);
    
    // 6. record content 
    std::fseek(f, sizeof(AResourceHead), SEEK_SET);
    WriteBytes(f, buffer, header.count * header.size);
    delete[] buffer;
    std::fclose(f);
    time_t end = time(nullptr);
    GOOGLE_LOG(INFO)<< "Take times: "<< (double)(end-start)<<" Seconds";
#endif
    return true;
}

bool Application::LoadSetting(const std::string& path, int argc, char* argv[]) {
    if (argc > 1) {
        m_argc = argc;    
        m_argv = argv;
        return true;
    }

    this->program_name = argv[0];
    json settings;
    std::ifstream jfile(path);
    if (!jfile.is_open()) {
        GOOGLE_LOG(ERROR) << "Could not open the file";
        return false;
    }

    try {
        jfile >> settings;
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) << "Fail parsing files: " <<  path <<  ", " <<  e.what(); 
        return false;
    }

    try {
        conf.files = settings["files"].get<std::vector<std::string>>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) << "When loading param files. " << e.what(); 
        return false;
    }

    try {
        conf.folders = settings["folders"].get<std::vector<std::string>>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param folders. " << e.what(); 
        return false;
    }

    try {
        conf.xlsx_folder = settings["xlsx_folder"].get<std::string>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param xlsx_folder. " << e.what(); 
        return false;
    }

    try {
        conf.output_folder = settings["output_folder"].get<std::string>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param output_folder. " << e.what(); 
        return false;
    }

    return true;
}

void Application::RunCommandMode() {
    google::protobuf::compiler::CommandLineInterface cli;
    cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", aresconvert_generator.get(), "Generate AResConvert source and header.");
    cli.Run(m_argc, m_argv);
}

void Application::Run() {
    if (m_argc > 1) {
        RunCommandMode();
        return;
    }

    RunBrowserMode();
}

void Application::RunBrowserMode() {
    std::vector<std::string> args = {
        "--aresconvert_out=protocol",
        "--aresconvert_opt=resource", 
        "-I=./",
    };

    const int argument_option_len = 128;
    for (auto& str: args) {
        auto len = str.size() + argument_option_len;
        plugin_arguments.push_back(std::make_unique<char[]>(len));
        snprintf(plugin_arguments.back().get(), len, "%s", str.c_str());
    }

    for (auto& str: conf.folders) {
        auto len = str.size() + argument_option_len;
        plugin_arguments.push_back(std::make_unique<char[]>(len));
        snprintf(plugin_arguments.back().get(), len, "-I=%s", str.c_str());
    }   

    for (auto& str: conf.files) {
        auto len = str.size() + argument_option_len;
        plugin_arguments.push_back(std::make_unique<char[]>(len));
        snprintf(plugin_arguments.back().get(), len, "%s", str.c_str());
    }

    auto size = plugin_arguments.size();
    char** argv = new char*[size + 1];
    int argc = 0;
    argv[argc++] = program_name;
    for (auto& cstr : plugin_arguments) {
        argv[argc++] = cstr.get();
    }

    // 生成资源列表 
    google::protobuf::compiler::CommandLineInterface cli;
    cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", aresconvert_generator.get(), "Generate AResConvert source and header.");
    cli.Run(argc, argv);
    delete argv;
}

json Application::Convert(const json& req) {
    std::string res_name = req[0]["res_name"].get<std::string>();
    sprintf(plugin_arguments[1].get(), "--aresconvert_opt=convert:%s", res_name.c_str());

    auto size = plugin_arguments.size();
    char** argv = new char*[size + 1];
    int argc = 0;
    argv[argc++] = program_name;
    for (auto& cstr : plugin_arguments) {
        argv[argc++] = cstr.get();
    }

    // 生成资源列表 
    google::protobuf::compiler::CommandLineInterface cli;
    cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", aresconvert_generator.get(), "Generate AResConvert source and header.");
    cli.Run(argc, argv);

    json result;
    if (!SerializeToBin(res_name)) {
        result["message"] = "fail";
        result["code"] == -1;
    }   else {
        result["message"] = "success";
        result["code"] == 0;
    }
    return result;
}

json Application::GetErrMessage(const json& req) {
    json result;
    result["code"] = 0;
    return result;
}

json Application::GetResourceNameAll(const json& req) {
    json result;
    result["result"] = aresconvert_generator->GetResourceNameAll();
    result["code"] = 0;
    return result; 
}

json Application::Refresh(const json& req) {
    json result;
    conf = Settings();
    err_message.clear();
    plugin_arguments.clear();
    aresconvert_generator = std::make_unique<AResConvertGenerator>();
    if (LoadSetting("setup.json", m_argc, m_argv)) {
        Run();
    }

    result["code"] = 0;
    result["message"] = "success";
    return result; 
}

Application::Application() {
    unsigned int x = 0x76543210;
    char *c = (char*) &x; 
    m_is_little_endian = (*c == 0x10);
}
