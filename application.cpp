#include <cstddef>
#include <filesystem>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <alibabacloud/oss/OssClient.h>
#include <ctime>
#include <thread>
#include <future>
#include <google/protobuf/compiler/command_line_interface.h>
#include "application.h"
#include "AEndiannessHelper.h"
#include "md5.h"
#include "helpers.h"
#include <filesystem>
using namespace AlibabaCloud::OSS;

#ifdef _WIN32
template<typename T>
bool Application::GetNumberCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val) {
    if (index >= row.size()) {
        val = "0";
        return true;
    }

    const OpenXLSX::XLCellValue& cell = row[index];
    if  (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = std::to_string(cell.get<T>());
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = "0";
    }   else {
        GOOGLE_LOG(WARNING) << "Invalid cell.type:" << (int)cell.type() << ", cell.col:" << GetCellCol(index);
        return false;
    }
    return true;
}

template<typename T>
bool Application::GetLargeNumberCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val) {
    if (index >= row.size()) {
        val = "\"0\"";
        return true; 
    }

    const OpenXLSX::XLCellValue& cell = row[index];
    if  (cell.type() == OpenXLSX::XLValueType::Integer) {
        val = "\"" + std::to_string(cell.get<T>()) + "\"";
    }   else if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = "\"0\"";
    }   else {
        GOOGLE_LOG(WARNING) << "Invalid cell.type:" << (int)cell.type() << "cell.col:" << GetCellCol(index);
        return false;
    }
    return true;
}

json Application::Upload(const json& req) {
    std::string AccessKeyId = conf.access_key_id;
    std::string AccessKeySecret = conf.access_key_secret;
    std::string Endpoint = conf.oss_end_point;
    std::string BucketName = "lifesim";
    std::string res_name = req[0]["res_name"].get<std::string>();
    std::filesystem::path p(conf.output_folder + "/" + aresconvert_generator->FindBinFileNameByMessageName(res_name));
    std::string filepath = p.string();
    std::string filename = p.filename().string();
    std::string ObjectName = conf.cloud_directory + filename;

    InitializeSdk();

    ClientConfiguration oss_conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, oss_conf);
    GOOGLE_LOG(INFO) << "Uploading  To Bucket:" << BucketName << " [" << filepath << "]  => [" << ObjectName << "]";
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(filepath, std::ios::in | std::ios::binary);
    if (!content->good()) {
        /* 释放网络等资源。*/
        ShutdownSdk();
        json result;
        result["code"] = -1;
        result["message"] = "Can't open such file: " + filepath;
        return result;
    }

    PutObjectRequest request(BucketName, ObjectName, content);
    /*（可选）请参见如下示例设置访问权限ACL为私有（private）以及存储类型为标准存储（Standard）。*/
    //request.MetaData().addHeader("x-oss-object-acl", "private");
    //request.MetaData().addHeader("x-oss-storage-class", "Standard");

    auto outcome = client.PutObject(request);

    if (!outcome.isSuccess()) {
        GOOGLE_LOG(INFO) << "PutObject fail" <<
        ",code:" << outcome.error().Code() <<
        ",message:" << outcome.error().Message() <<
        ",requestId:" << outcome.error().RequestId();
        json result;
        result["code"] = 0;
        result["message"] = outcome.error().Message();
        result["requstId"] = outcome.error().RequestId();
        return -1;
    }
    GOOGLE_LOG(INFO) << "Succeed";

    /* 释放网络等资源。*/
    ShutdownSdk();

    json result;
    result["code"] = 0;
    result["message"] = "success";
    return result;
}

bool Application::GetStringCellVal(const std::vector<OpenXLSX::XLCellValue>& row, int index, std::string& val) {
    if (index >= row.size()) {
        val = "\"\"";
        return true;
    }

    const OpenXLSX::XLCellValue& cell = row[index];
    if (cell.type() == OpenXLSX::XLValueType::Empty) {
        val = "\"\"";
    }   else if (cell.type() != OpenXLSX::XLValueType::Error) {
        if (cell.type() == OpenXLSX::XLValueType::String) {
            val = "\"" + cell.get<std::string>() + "\""; 
        }   else {
            val =  "\"" + std::to_string(cell.get<int64_t>()) + "\"";
        }
    }   else {
        GOOGLE_LOG(WARNING) << "Invalid cell.type:" << (int)cell.type() << "cell.col:" << GetCellCol(index);
        return false; 
    }
    return true;
}

bool Application::WriteInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

bool Application::WriteUInt32Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

bool Application::WriteInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

bool Application::WriteUInt16Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

bool Application::WriteInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

inline bool Application::WriteUInt64Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

inline bool Application::WriteInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

inline bool Application::WriteUInt8Cell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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


inline bool Application::WriteDoubleCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

inline bool Application::WriteFloatCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
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

inline bool Application::WriteStringCell(char* record_buffer, const ATableFieldMeta& field_meta, const OpenXLSX::XLCellValue& cell) {
    std::string val;
    if (cell.type() == OpenXLSX::XLValueType::Empty) {
    }   else if (cell.type() != OpenXLSX::XLValueType::Error) {
        if (cell.type() == OpenXLSX::XLValueType::String) {
            val = cell.get<std::string>();
        }   else {
            val = std::to_string(cell.get<int64_t>());
        }
        if (val.size() >= field_meta.size) {
            GOOGLE_LOG(ERROR) << "string: " << val << " is greater equal to " << field_meta.size;
            return false;
        }
    }   else {
        return false;
    }
    WriteString(record_buffer + field_meta.offset, val);
    return true;
}
#endif

bool Application::SerializeToJson(const std::string& res_name) {
#ifdef _WIN32
    std::string xlsx_name = aresconvert_generator->FindXLSXFileNameByMessageName(res_name);
    std::string sheet_name = aresconvert_generator->FindSheetNameByMessageName(res_name);
    std::string bin_name = aresconvert_generator->FindBinFileNameByMessageName(res_name);
    std::string key_name = aresconvert_generator->FindKeyNameByMessageName(res_name);

    try {
        time_t start = std::time(nullptr);
        OpenXLSX::XLDocument doc;
        doc.open(conf.xlsx_folder + "/" + xlsx_name);
        //TODO catch exception
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

        int row_count = wks.rowCount();
        std::string out_file = conf.output_folder + "/" + bin_name;
        std::FILE* f = std::fopen(out_file.c_str(), "w+");
        if (f == nullptr) {
            GOOGLE_LOG(ERROR) << "fail to open " << out_file;
            return false;
        }

        std::unordered_map<std::string, int> field_to_index_mapping;
        const std::vector<OpenXLSX::XLCellValue>& values = wks.row(first_row).values();
        int idx = 0;
        for (const auto& cell: values) {
            std::string name = cell.get<std::string>();
            if (name == "") {
                idx++;
                continue;
            }

            if (field_to_index_mapping.find(name) != field_to_index_mapping.end()) {
                GOOGLE_LOG(ERROR) << "duplicate name:" << name;
                return false;
            }
            field_to_index_mapping.emplace(name, idx++);
        }

        auto iter = field_to_index_mapping.find(key_name);
        if (iter == field_to_index_mapping.end()) {
            GOOGLE_LOG(WARNING) << "can't find key name" << key_name;
            return false;
        }
        int key_index = iter->second;

        std::unordered_map<std::string, AMessageMeta> message_meta_map = aresconvert_generator->GetMessageMeta();
        auto message_meta_iter = message_meta_map.find(res_name);
        if (message_meta_iter == message_meta_map.end()) {
            GOOGLE_LOG(ERROR) << "Fail to find message_meta:" << res_name;
            return false;
        }

        const AMessageMeta& meta = message_meta_iter->second;
        const std::map<std::string, AFieldMeta>& meta_data = meta.GetMetaData();
        auto field_meta_iter = meta_data.find(key_name);
        if (field_meta_iter == meta_data.end()) {
            GOOGLE_LOG(ERROR) << "Fail to find field_meta:" << key_name;
            return false; 
        }

        int rowNo = 0; 
        std::fwrite("{", 1, 1, f);
        for (const auto& rowData : wks.rows()) {
            rowNo++;
            if (rowNo <= first_row) {
                continue;
            }

            std::string key;
            const std::vector<OpenXLSX::XLCellValue>& values = rowData.values();
            if (!GetPrimitiveValForJson(field_meta_iter->second.field_type, values, key_index, key)) {
                GOOGLE_LOG(INFO) << "fail to get key:" << key << " in row:" << rowNo;
                return false;
            }

            key = "\"" + key +"\":";
            std::fwrite(key.c_str(), key.size(), 1, f);

            if (!WriteJsonToFile(f, message_meta_map, values, field_to_index_mapping, res_name, "")) {
                GOOGLE_LOG(INFO) << "row:" << rowNo;
                std::fclose(f);
                return false;
            }

            if (rowNo < row_count) {
                std::fwrite(",", 1, 1, f);
            }
        }
        std::fwrite("}", 1, 1, f);
        std::fclose(f);
    }   catch (const std::exception& e) {
        GOOGLE_LOG(ERROR) << e.what();
    }

#endif
    return true;
}

bool Application::GetPrimitiveValForJson(FIELDTYPE field_type, const std::vector<OpenXLSX::XLCellValue>& row_data, int col_index, std::string& val)
{
    bool ret = false;
    switch (field_type) {
        case FIELDTYPE_INT16: {
            ret = GetNumberCellVal<int16_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_UINT16: {
            ret = GetNumberCellVal<uint16_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_UINT32: {
            ret = GetNumberCellVal<uint32_t>(row_data, col_index, val);
        };break;

        case FIELDTYPE_INT8: {
            ret = GetNumberCellVal<int8_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_UINT8: {
            ret = GetNumberCellVal<uint8_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_DOUBLE: {
            ret = GetNumberCellVal<float>(row_data, col_index, val);
        };break;
        case FIELDTYPE_FLOAT: {
            ret = GetNumberCellVal<float>(row_data, col_index, val);
        };break;
        case FIELDTYPE_INT32: {
            ret = GetNumberCellVal<int32_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_INT64: {
            ret = GetNumberCellVal<int64_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_UINT64: {
            ret = GetNumberCellVal<uint64_t>(row_data, col_index, val);
        };break;
        case FIELDTYPE_STRING: {
            ret = GetStringCellVal(row_data, col_index, val);
        };break;
        default:
            GOOGLE_LOG(ERROR) << "Invalid field_type:" << (int)field_type << ", col:" << GetCellCol(col_index);
            ret = false;
            break;
    }
    return ret;
}

bool Application::WriteJsonToFile(std::FILE* f, std::unordered_map<std::string, AMessageMeta>& message_meta_map, const std::vector<OpenXLSX::XLCellValue>& row_data, const std::unordered_map<std::string, int>& field_to_index_mapping, const std::string& name, const std::string& prefix) {

    auto message_meta_iter = message_meta_map.find(name);
    if (message_meta_iter == message_meta_map.end()) {
        GOOGLE_LOG(ERROR) << "Fail to find message_meta:" << name;
        return false;
    }

    const AMessageMeta& meta = message_meta_iter->second;
    const std::map<std::string, AFieldMeta>& meta_data = meta.GetMetaData();
    std::fwrite("{", 1, 1, f);
    int index = 0;
    
    for (auto iter: meta_data) {
        std::string key = "\"" + iter.second.name + "\"";
        std::fwrite(key.c_str(), key.size(), 1, f);
        std::fwrite(":", 1, 1, f);
        if (iter.second.repeated) {
            std::fwrite("[", 1, 1, f);
            for (int i = 0; i < iter.second.count; i++) {
                std::string col_name = prefix == "" ? 
                    iter.second.name + std::to_string(i+1) : prefix + "#"+iter.second.name + std::to_string(i+1);
                int col_index = 0;
                if (iter.second.field_type != FIELDTYPE_INVALID && iter.second.field_type != FIELDTYPE_MESSAGE) {
                    auto col_iter = field_to_index_mapping.find(col_name);
                    if (col_iter == field_to_index_mapping.end()) {
                        GOOGLE_LOG(ERROR) << "Fail to find col name " << col_name;
                        return false;
                    }
                    col_index = col_iter->second;
                }

                if (iter.second.field_type == FIELDTYPE_MESSAGE) {
                    if (!WriteJsonToFile(f, message_meta_map, row_data, field_to_index_mapping, iter.second.type_name, col_name)) {
                        GOOGLE_LOG(WARNING) << "type_name: " << iter.second.type_name;   
                        return false;
                    };
                }   else {
                    std::string val;
                    if (!GetPrimitiveValForJson(iter.second.field_type, row_data, col_index, val)) {
                        GOOGLE_LOG(WARNING) << "WritePrimitiveValToFile fail";
                        return false;
                    }
                    std::fwrite(val.c_str(), val.size(), 1, f);
                }

                if (i + 1 < iter.second.count) {
                    std::fwrite(",", 1, 1, f);
                }
            }

            std::fwrite("]", 1, 1, f);
        }
        else {
            std::string col_name = prefix == "" ? iter.second.name : prefix + "#"+iter.second.name;
            auto col_iter = field_to_index_mapping.find(col_name);
            int col_index = 0;
            if (iter.second.field_type != FIELDTYPE_INVALID && iter.second.field_type != FIELDTYPE_MESSAGE) {
                if (col_iter == field_to_index_mapping.end()) {
                    GOOGLE_LOG(ERROR) << "Fail to find col name " << col_name;
                    return false;
                }
                col_index = col_iter->second; 
            }

            if (iter.second.field_type == FIELDTYPE_MESSAGE) {
                if (!WriteJsonToFile(f, message_meta_map, row_data, field_to_index_mapping, iter.second.type_name, col_name)) {
                    GOOGLE_LOG(WARNING) << "type_name: " << iter.second.type_name;   
                    return false;
                };
            }   else {
                std::string val;
                if (!GetPrimitiveValForJson(iter.second.field_type, row_data, col_index, val)) {
                    GOOGLE_LOG(WARNING) << "WritePrimitiveValToFile fail";
                    return false;
                }
                std::fwrite(val.c_str(), val.size(), 1, f);
            }
        }
        if (index + 1 < meta_data.size()) {
            std::fwrite(",", 1, 1, f);
        }
        index++;
    }

    std::fwrite("}", 1, 1, f);
    return true;
}


bool Application::SerializeToBin(const std::string& res_name) {
#ifdef _WIN32
    std::string xlsx_name = aresconvert_generator->FindXLSXFileNameByMessageName(res_name);
    std::string sheet_name = aresconvert_generator->FindSheetNameByMessageName(res_name);
    std::string bin_name = aresconvert_generator->FindBinFileNameByMessageName(res_name);

    time_t start = std::time(nullptr);
    OpenXLSX::XLDocument doc;
    try {
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
        std::map<std::string, ATableFieldMeta> field_metas = message_meta.GetFieldMetas();
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
        WriteBytes(f, message_meta.GetMetaMD5());
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
                const ATableFieldMeta& field_meta = columns[col].GetMeta();
                if (!columns[col].IsValid()) {
                    continue;
                }

                const auto& cell = values[col];
                bool result = true;
                const auto field_type = field_meta.field_type;
                switch (field_type) {
                    case FIELDTYPE_INT32: { result = WriteInt32Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_UINT32: { result = WriteUInt32Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_INT16: { result = WriteInt16Cell(offset, field_meta, cell);} break;   
                    case FIELDTYPE_UINT16: { result = WriteUInt16Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_INT64: { result = WriteInt64Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_UINT64: { result = WriteUInt64Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_INT8: { result = WriteInt8Cell(offset, field_meta, cell);} break;   
                    case FIELDTYPE_UINT8: { result = WriteUInt8Cell(offset, field_meta, cell);} break;
                    case FIELDTYPE_FLOAT: { result = WriteFloatCell(offset, field_meta, cell);} break;
                    case FIELDTYPE_DOUBLE: { result = WriteDoubleCell(offset, field_meta, cell);} break;
                    case FIELDTYPE_STRING: { result = WriteStringCell(offset, field_meta, cell);} break;
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

        std::fseek(f, offsetof(AResourceHead, meta_md5), SEEK_SET);
        WriteBytes(f, message_meta.GetMetaMD5());

        // 6. record content 
        std::fseek(f, sizeof(AResourceHead), SEEK_SET);
        WriteBytes(f, buffer, header.count * header.size);
        delete[] buffer;
        std::fclose(f);
        time_t end = time(nullptr);
        GOOGLE_LOG(INFO)<< "Take times: "<< (double)(end-start)<<" Seconds";

    }   catch (const std::exception& e) {
        GOOGLE_LOG(ERROR) << e.what();
        return false;
    }

#endif
    return true;
}

bool Application::LoadSetting(const std::string& path, int argc, char* argv[]) {
    m_argc = argc;    
    m_argv = argv;
    if (argc > 1) {
        return true;
    }

    this->program_name = argv[0];
    std::ifstream jfile(path);
    if (!jfile.is_open()) {
        GOOGLE_LOG(ERROR) << "Could not open the file";
        return false;
    }

    try {
        jfile >> conf_json;
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) << "Fail parsing files: " <<  path <<  ", " <<  e.what(); 
        return false;
    }

    try {
        conf.files = conf_json["files"].get<std::vector<std::string>>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) << "When loading param files. " << e.what(); 
        return false;
    }

    try {
        conf.folders = conf_json["folders"].get<std::vector<std::string>>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param folders. " << e.what(); 
        return false;
    }

    try {
        conf.xlsx_folder = conf_json["xlsx_folder"].get<std::string>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param xlsx_folder. " << e.what(); 
        return false;
    }

    try {
        conf.output_folder = conf_json["output_folder"].get<std::string>();
    }   catch (json::exception& e) {
        GOOGLE_LOG(ERROR) <<  "When loading param output_folder. " << e.what(); 
        return false;
    }

    try {
        conf.bucket_name = conf_json["bucket_name"].get<std::string>();
    }   catch (json::exception& e) {
        //GOOGLE_LOG(WARNING) <<  "When loading param cloud_directory. " << e.what(); 
    }

    try {
        conf.oss_end_point = conf_json["oss_end_point"].get<std::string>();
    }   catch (json::exception& e) {
        //GOOGLE_LOG(WARNING) <<  "When loading param cloud_directory. " << e.what(); 
    }

    try {
        conf.cloud_directory = conf_json["cloud_directory"].get<std::string>();
    }   catch (json::exception& e) {
        //GOOGLE_LOG(WARNING) <<  "When loading param cloud_directory. " << e.what(); 
    }

    try {
        conf.access_key_id = conf_json["access_key_id"].get<std::string>();
    }   catch (json::exception& e) {
        //GOOGLE_LOG(WARNING) <<  "When loading param cloud_directory. " << e.what(); 
    }

    try {
        conf.access_key_secret = conf_json["access_key_secret"].get<std::string>();
    }   catch (json::exception& e) {
        //GOOGLE_LOG(WARNING) <<  "When loading param cloud_directory. " << e.what(); 
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

json Application::ConvertBin(const json& req) {
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
        GOOGLE_LOG(INFO) << "Convert Bin failed!!!";
    }   else {
        result["message"] = "success";
        result["code"] == 0;
        GOOGLE_LOG(INFO) << "Convert Bin failed!!!";
    }
    return result;
}

json Application::ConvertJson(const json& req) {
    std::string res_name = req[0]["res_name"].get<std::string>();
    sprintf(plugin_arguments[1].get(), "--aresconvert_opt=convert_json:%s", res_name.c_str());

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
    if (!SerializeToJson(res_name)) {
        result["message"] = "fail";
        result["code"] == -1;
        GOOGLE_LOG(INFO) << "Convert Json failed!!!";
    }   else {
        result["message"] = "success";
        result["code"] == 0;
        GOOGLE_LOG(INFO) << "Convert Json " + res_name + " succeed!!!";
    }
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
    GOOGLE_LOG(INFO) << "Refresh succeed.";
    result["code"] = 0;
    result["message"] = "success";
    return result; 
}

json Application::GetConf(const json& req) {
    json result;
    result["code"] = 0;
    result["message"] = "success";
    result["conf"] = conf_json;
    return result;
}

Application::Application() {
}
