#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstddef>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include "aresconvert_generator.h"
#include "webview.h"
#include "helpers.h"
#include "OpenXLSX.hpp"
#include "endian.h"

using namespace OpenXLSX;

struct Settings {
    std::vector<std::string> files;
    std::vector<std::string> folders;
    std::string xlsx_folder;
    std::string output_folder;
};

struct ColData {
    int col;
    ColData(int reqcol): col(reqcol){};
};

inline int32_t WriteInt32(std::FILE* f, int32_t val) {
    int32_t n_val = htobe32(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteUInt32(std::FILE* f, uint32_t val) {
    uint32_t n_val = htobe32(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteInt16(std::FILE* f, int16_t val) {
    int16_t n_val = htobe16(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteUInt16(std::FILE* f, uint16_t val) {
    uint16_t n_val = htobe16(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteInt64(std::FILE* f, int64_t val) {
    int64_t n_val = htobe64(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteUInt64(std::FILE* f, uint64_t val) {
    uint64_t n_val = htobe64(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteInt8(std::FILE* f, int8_t val) {
    std::fwrite(&val, 1, sizeof(val), f);
    return sizeof(val);
}

inline int32_t WriteUInt8(std::FILE* f, uint8_t val) {
    std::fwrite(&val, 1, sizeof(val), f);
    return sizeof(val);
}

inline int32_t WriteDouble(std::FILE* f, double val) {
    double n_val = htobe64(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteFloat(std::FILE* f, float val) {
    float n_val = htobe32(val);
    std::fwrite(&n_val, 1, sizeof(n_val), f);
    return sizeof(n_val);
}

inline int32_t WriteString(std::FILE* f, const std::string& val) {
    std::fwrite(val.c_str(), 1, val.size() + 1, f);
    return val.size() + 1;
}

struct Application {
    Settings conf;
    char* program_name;
    std::vector<std::string> err_message;
    std::vector<std::unique_ptr<char[]>> plugin_arguments;
    AResConvertGenerator aresconvert_generator;

    bool SerializeToBin(const std::string& res_name) {
        std::string xlsx_name = aresconvert_generator.FindXLSXFileNameByMessageName(res_name);
        std::string sheet_name = aresconvert_generator.FindSheetNameByMessageName(res_name);
        std::string bin_name = aresconvert_generator.FindBinFileNameByMessageName(res_name);
        XLDocument doc;
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
        std::FILE* f = std::fopen(out_file.c_str(), "w+");

        int row_count = wks.rowCount();
        int col_count = wks.columnCount();

        std::vector<std::pair<FieldMeta, ColData>> columns;
        for (const auto& col: aresconvert_generator.GetColsName()) {
            int size = columns.size();
            for (int i = 1; i <= col_count; ++i) {
                if (wks.cell(first_row, i).value() == col.name) {
                    columns.emplace_back(col, ColData(i));
                    break;
                }
            }
            if (columns.size() == size) {
                GOOGLE_LOG(ERROR) << "Can not find col: " << col.name;
                return false;
            }
        }

        AResourceHead header;
        header.total_size += sizeof(AResourceHead);
        std::fseek(f, sizeof(AResourceHead), SEEK_SET);
        for (const auto& col: columns) {
            header.col_num++;
            header.col_info_size += WriteString(f, col.first.name);
            header.col_info_size += WriteInt32(f, col.first.size);
            header.record_size += col.first.size;
        }
        std::fseek(f, offsetof(AResourceHead, col_num), SEEK_SET);
        WriteInt32(f, header.col_num);
        std::fseek(f, offsetof(AResourceHead, col_info_size), SEEK_SET);
        WriteInt32(f, header.col_info_size);
        std::fseek(f, offsetof(AResourceHead, record_size), SEEK_SET);
        WriteInt32(f, header.record_size);

        header.total_size += header.col_info_size;
        std::cout << "size: " << header.total_size << std::endl;
        std::fseek(f, header.total_size, SEEK_SET);
        for (int i = first_row + 1; i <= row_count; i++) {
            for (const auto& col: columns) {
                switch (col.first.field_type){
                    case FIELDTYPE_INT32: {
                        int32_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type int32";
                            return false;
                        }   else {
                            val = cell.value().get<int32_t>();
                        }
                        header.total_size += WriteInt32(f, val);
                    }   
                    break;
                    case FIELDTYPE_UINT32: {
                        uint32_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type uint32";
                            return false;
                        }   else {
                            val = cell.value().get<uint32_t>();
                        }
                        header.total_size += WriteUInt32(f, val);
                    }
                    break;
                    case FIELDTYPE_INT16: {
                        int16_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type int16";
                            return false;
                        }   else {
                            val = cell.value().get<int16_t>();
                        }
                        header.total_size += WriteInt16(f, val);
                    }   
                    break;
                    case FIELDTYPE_UINT16: {
                        uint16_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type uint16";
                            return false;
                        }   else {
                            val = cell.value().get<uint16_t>();
                        }
                        header.total_size += WriteUInt16(f, val);
                    }
                    break;
                    case FIELDTYPE_INT64: {
                        int64_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type int64";
                            return false;
                        }   else {
                            val = cell.value().get<int64_t>();
                        }
                        header.total_size += WriteInt64(f, val);
                    }   
                    break;
                    case FIELDTYPE_UINT64: {
                        uint64_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type uint64";
                            return false;
                        }   else {
                            val = cell.value().get<uint64_t>();
                        }
                        header.total_size += WriteUInt64(f, val);
                    }
                    break;
                    case FIELDTYPE_INT8: {
                        int8_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type int8";
                            return false;
                        }   else {
                            val = cell.value().get<int8_t>();
                        }
                        header.total_size += WriteInt8(f, val);
                    }   
                    break;
                    case FIELDTYPE_UINT8: {
                        uint8_t val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Integer) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type uint8";
                            return false;
                        }   else {
                            val = cell.value().get<uint8_t>();
                        }
                        header.total_size += WriteUInt8(f, val);
                    }
                    break;
                    case FIELDTYPE_FLOAT: {
                        float val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Float) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type float";
                            return false;
                        }   else {
                            val = cell.value().get<float>();
                        }
                        header.total_size += WriteFloat(f, val);
                    }   
                    break;
                    case FIELDTYPE_DOUBLE: {
                        double val = 0;
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::Float) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type double";
                            return false;
                        }   else {
                            val = cell.value().get<double>();
                        }
                        header.total_size += WriteDouble(f, val);
                    }
                    break;
                    case FIELDTYPE_STRING: {
                        std::string val = "";
                        const auto& cell = wks.cell(i, col.second.col);
                        if (cell.value().type() == XLValueType::Empty) {
                        }   else if (cell.value().type() != XLValueType::String) {
                            GOOGLE_LOG(ERROR) << cell.cellReference().address() << " does not match type double";
                            return false;
                        }   else {
                            val = cell.value().get<std::string>();
                        }
                        
                        if (val.size() >= col.first.size) {
                            GOOGLE_LOG(ERROR) << "string: " << val << " is greater equal to " << col.first.size;
                            return false;
                        }
                        header.total_size += WriteString(f, val);
                        int padded = col.first.size - val.size() - 1;
                        header.total_size += padded;
                        std::fseek(f, padded, SEEK_CUR);
                    }
                    break;
                }
            }
        }
        std::fseek(f, offsetof(AResourceHead, total_size), SEEK_SET);
        WriteInt32(f, header.total_size);
        std::fclose(f);

        return true;
    }

    bool LoadSetting(const std::string& path, char* program_name) {
        this->program_name = program_name;
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

    void Run() {

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
        cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", &aresconvert_generator, "Generate AResConvert source and header.");
        cli.Run(argc, argv);
        delete argv;
    }
    
    json Convert(const json& req) {
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
        cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", &aresconvert_generator, "Generate AResConvert source and header.");
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

    json GetErrMessage(const json& req) {
        json result;
        return result;
    }

    json GetResourceNameAll(const json& req) {
        json result;
        result["result"] = aresconvert_generator.GetResourceNameAll();
        return result; 
    }
};

Application app;

template <typename Func, typename Obj>
auto HandlerWrapper(Func f, Obj* obj)
{
    
    auto handler = [=](const std::string &s) -> std::string {
        std::stringstream sstream;
        auto old = std::cerr.rdbuf(sstream.rdbuf());
        json result;
        try
        {
            json req = json::parse(s);
            result = (obj->*f)(std::forward<json>(req));
            if (!result.contains("code")) {
                result["code"] = 0;
            }
        }
        catch (json::parse_error& ex)
        {
            result["code"] = -1;
            std::string message = "parse error at byte " + ex.byte;
            GOOGLE_LOG(ERROR) << message;
        }

        // LogHandling
        const std::string& logs = sstream.str();
        if (logs.size() != 0) {
            result["code"] = -1;
            std::vector<std::string> out = stringSplit(logs, "\n");
            for (auto& x: out){
                result["log"].push_back(x);
            }
        }
        for (auto &x: obj->err_message) {
            result["log"].push_back(x);
        }
        obj->err_message.clear();
        std::cerr.rdbuf(old);
        return result.dump();

    };
    return handler;
}

void CustomLogHandler(LogLevel level, const char* filename, int line, const std::string& message) {
    const char* level_name = nullptr;
    switch (level) {
        case LOGLEVEL_ERROR:
            level_name = "error"; break;
        case LOGLEVEL_FATAL:
            level_name = "fatal"; break;
        case LOGLEVEL_WARNING:
            level_name = "warning"; break;
        default:
            level_name = "info"; break;
    }

    std::string log = std::string(filename) +  ":" + std::to_string(line) + ": " + level_name + ": " + message;
    if (level > LOGLEVEL_INFO) {
        app.err_message.push_back(log);
        std::cout << log << std::endl;
    }
    else {
        std::cout << log << std::endl; 
    }
}

int main(int argc, char* argv[]) {
    SetLogHandler(CustomLogHandler);
    if (app.LoadSetting("setup.json", argv[0])) {
        app.Run();
    }

    webview::webview w(true, nullptr, 1024, 768);
    w.bind("Convert", HandlerWrapper(&Application::Convert, &app));
    w.bind("GetResourceNameAll", HandlerWrapper(&Application::GetResourceNameAll, &app));
    w.bind("GetErrMessage", HandlerWrapper(&Application::GetErrMessage, &app));

    w.navigate("https://appassets.example/index.html");
    w.run();
    return 0;    
}

