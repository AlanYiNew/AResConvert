#ifndef __APPLICATION_
#define __APPLICATION_

#include "aresconvert_generator.h"
#include "md5.h"
#include "helpers.h"
#include "task_dispatcher.h"
#include <memory>
#include <cstdio>
#ifdef _WIN32
#include "OpenXLSX.hpp"
#endif

struct Settings {
    std::vector<std::string> files;
    std::vector<std::string> folders;
    std::string xlsx_folder;
    std::string output_folder;
    std::string cloud_directory;
    std::string bucket_name;
    std::string oss_end_point;
    std::string access_key_id;
    std::string access_key_secret;
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
    ~Application();

    std::vector<std::string> err_message;
    std::vector<std::unique_ptr<char[]>> plugin_arguments;
    std::unique_ptr<AResConvertGenerator> aresconvert_generator{std::make_unique<AResConvertGenerator>()};

    bool SerializeToBin(const std::string& res_name); 
    bool SerializeToJson(const std::string& res_name); 
    bool WriteJsonToFile(std::FILE* f, std::unordered_map<std::string, AMessageMeta>& message_meta_map, const std::vector<OpenXLSX::XLCellValue>&, const std::unordered_map<std::string, int>& field_to_index_mapping, const std::string& res_name, const std::string& col_name);
    bool GetPrimitiveValForJson(const AFieldMeta& field_meta, const std::vector<OpenXLSX::XLCellValue>& row_data, int col_index, std::string& val);

    bool LoadSetting(const std::string& path, int argc, char* argv[]); 
    bool SaveSetting(const std::string& path);
    void Run(); 

    template <typename Func, typename Resolver>
    auto HandlerWrapper(Func f, Resolver resolver)
    {
        auto handler = [=](const std::string &seq, const std::string &s, void* arg) -> void {
            try
            {
                json req = json::parse(s);
                m_task_dispatcher.dispatch([seq, req, f, arg, resolver, this](){
                    auto& log_stream = GetLogStream();
                    auto old_cerr = std::cerr.rdbuf();
                    auto old_cout = std::cout.rdbuf();
                    std::cerr.rdbuf(log_stream.rdbuf());
                    std::cout.rdbuf(log_stream.rdbuf());

                    json result = (this->*f)(req);
                    std::cout.rdbuf(old_cout);
                    std::cerr.rdbuf(old_cerr);

                    // LogHandling
                    const std::string& logs = log_stream.str();
                    if (logs.size() != 0) {
                        std::vector<std::string> out = stringSplit(logs, "\n");
                        for (auto& x: out) {
                            result["log"].push_back(x);
                        }
                    }
                    log_stream.str("");
                    int ret = 0;
                    if (!result["code"].is_null()) {
                        ret = result["code"].get<int>();
                    }
                    resolver->resolve(seq, ret, result.dump()); 
                });
            }
            catch (json::parse_error& ex)
            {
                json result;
                result["code"] = -1;
                GOOGLE_LOG(ERROR) << "parse error at byte " + ex.byte;
                auto w = (Resolver*)arg;
                resolver->resolve(seq, -1, result.dump()); 
            }
        };
        return handler;
    }

    json Upload(const json& req);
    json ConvertBin(const json& req);
    json ConvertJson(const json& req);
    json GetConf(const json& req);
    json GetResourceNameAll(const json& req); 
    json Refresh(const json& req);
    std::stringstream& GetLogStream() { return m_log_stream; };

private:
    int m_argc{};
    char** m_argv;
    char* program_name{};
    Settings conf{};
    json conf_json{}; 
    std::stringstream m_log_stream{};
    std::unique_ptr<MD5> m_md5{};
    void RunBrowserMode();
    void RunCommandMode();
    std::unordered_map<std::string, AEnumMeta> m_enum_meta_map;
    std::string m_setting_file_name = "setup.json";
    TaskDispatcher m_task_dispatcher{1}; 

#ifdef _WIN32
    bool WriteInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteEnumCell(char* record_buffer, const ATableFieldMeta& field_meta, const AEnumMeta& enum_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteUInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteDoubleCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteFloatCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);
    bool WriteStringCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell);

    template<typename T>
    bool GetLargeNumberCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val);
    template<typename T>
    bool GetNumberCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val);
    bool GetStringCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val);
    bool GetEnumCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, AEnumMeta& enum_meta, std::string& val);

#endif
};
#endif
