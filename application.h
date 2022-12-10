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
    AFieldMeta m_meta;
public:
    ColData(AFieldMeta meta, bool valid): m_meta(meta), m_valid(valid){};
    ColData(bool valid): m_valid(valid){};
    const AFieldMeta& GetMeta() { return m_meta; };
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
    bool m_is_little_endian{};
    char* program_name{};
    Settings conf{};
    std::stringstream m_log_stream{};
    std::unique_ptr<MD5> m_md5{};
    void RunBrowserMode();
    void RunCommandMode();
    int32_t WriteInt32(void* f, int32_t val); 
    int32_t WriteUInt32(void* f, uint32_t val);
    int32_t WriteInt16(void* f, int16_t val); 
    int32_t WriteUInt16(void* f, uint16_t val); 
    int32_t WriteInt64(void* f, int64_t val); 
    int32_t WriteUInt64(void* f, uint64_t val); 
    int32_t WriteInt8(void* f, int8_t val); 
    int32_t WriteUInt8(void* f, uint8_t val);
    int32_t WriteDouble(void* f, double val);
    int32_t WriteFloat(void* f, float val);
    int32_t WriteBytes(void* f, const std::string& val);
    int32_t WriteString(void* f, const std::string& val);
    int32_t WriteInt32(std::FILE* f, int32_t val);
    int32_t WriteUInt32(std::FILE* f, uint32_t val);
    int32_t WriteInt16(std::FILE* f, int16_t val);
    int32_t WriteUInt16(std::FILE* f, uint16_t val);
    int32_t WriteInt64(std::FILE* f, int64_t val);
    int32_t WriteUInt64(std::FILE* f, uint64_t val);
    int32_t WriteInt8(std::FILE* f, int8_t val);
    int32_t WriteUInt8(std::FILE* f, uint8_t val);
    int32_t WriteDouble(std::FILE* f, double val);
    int32_t WriteFloat(std::FILE* f, float val);
    int32_t WriteBytes(std::FILE* f, const std::string& val);
    int32_t WriteBytes(std::FILE* f, char* buffer, int32_t size);
    int32_t WriteString(std::FILE* f, const std::string& val);
#ifdef _WIN32
    bool WriteInt32Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt32Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt8Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt8Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt16Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt16Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt64Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt64Cell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteDoubleCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteFloatCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteStringCell(char* record_buffer, const AFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
#endif
};
#endif
