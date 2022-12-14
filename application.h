#ifndef __APPLICATION_
#define __APPLICATION_

#include "aresconvert_generator.h"
#include "md5.h"
#include <memory>
#ifdef _WIN32
#include "OpenXLSX.hpp"
#endif

struct Settings {
    std::vector<std::string> files;
    std::vector<std::string> folders;
    std::string xlsx_folder;
    std::string output_folder;
};

class ColData {
    bool m_valid;
    ATableFieldMeta m_meta;
public:
    ColData(ATableFieldMeta meta, bool valid): m_meta(meta), m_valid(valid){};
    ColData(bool valid): m_valid(valid){};
    const ATableFieldMeta& GetMeta() { return m_meta; };
    bool IsValid() { return m_valid; };
};

struct Application {
    Application();

    std::vector<std::string> err_message;
    std::vector<std::unique_ptr<char[]>> plugin_arguments;
    std::unique_ptr<AResConvertGenerator> aresconvert_generator{std::make_unique<AResConvertGenerator>()};

    bool SerializeToBin(const std::string& res_name); 

    bool LoadSetting(const std::string& path, int argc, char* argv[]); 
    void Run(); 

    json Convert(const json& req);
    json GetErrMessage(const json& req);
    json GetResourceNameAll(const json& req); 
    json Refresh(const json& req);
    std::stringstream& GetLogStream() { return m_log_stream; };

private:
    int m_argc{};
    char** m_argv;
    char* program_name{};
    Settings conf{};
    std::stringstream m_log_stream{};
    std::unique_ptr<MD5> m_md5{};
    void RunBrowserMode();
    void RunCommandMode();

#ifdef _WIN32
    bool WriteInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteDoubleCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteFloatCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteStringCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
#endif
};
#endif
