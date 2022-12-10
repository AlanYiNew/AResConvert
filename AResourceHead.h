#ifndef __ARESOURCE_HEAD_
#define __ARESOURCE_HEAD_

#include <cstdint>
#include <cstddef>
#include "md5.h"
#include <unordered_map>
#include <map>

struct AResourceHead{
    int32_t total_size{};
    int32_t size{}; // size of record in bytes
    int32_t count{}; // number of records
    uint8_t meta_md5[16]{};
    uint8_t content_md5[16]{};
};

enum FIELDTYPE {
    FIELDTYPE_INVALID = 0,
    FIELDTYPE_INT32 = 1,
    FIELDTYPE_UINT32 = 2,
    FIELDTYPE_INT64 = 3,
    FIELDTYPE_UINT64 = 4,
    FIELDTYPE_INT16 = 5,
    FIELDTYPE_UINT16 = 6,
    FIELDTYPE_INT8 = 7,
    FIELDTYPE_UINT8 = 8,
    FIELDTYPE_STRING = 9,
    FIELDTYPE_DOUBLE = 10,
    FIELDTYPE_FLOAT = 11,
};

struct AFieldMeta {
    std::string name;
    int32_t size;
    int32_t offset;
    FIELDTYPE field_type;
    AFieldMeta() =  default;
    AFieldMeta(const std::string& field, FIELDTYPE field_type, int32_t size, int32_t offset): field_type(field_type), name(field), size(size), offset(offset) {};
};

struct AMessageMeta {
    int32_t m_size{}; // size of bytes of the records
    std::map<std::string, AFieldMeta> meta_data;
    void CreateField(const std::string& name, FIELDTYPE field_type, int32_t size) {
        meta_data.emplace(name, AFieldMeta{name, field_type, size, m_size});
        m_size += size;
    };
    const AFieldMeta* GetFieldMeta(const std::string& name) const {
        auto iter = meta_data.find(name);
        if (iter != meta_data.end()) {
            return &iter->second;
        }
        return nullptr;
    }

    const std::map<std::string, AFieldMeta>& GetFieldMetas() const {
        return meta_data;
    }

    int32_t GetSize() { return m_size; };

    std::string GetMD5() {
        MD5 md5;
        for (auto& iter : meta_data) {
            auto& field_meta = iter.second;
            md5.update(field_meta.name.c_str(), field_meta.name.size());
            md5.update(&field_meta.size, sizeof(field_meta.size));
            md5.update(&field_meta.field_type, sizeof(field_meta.field_type));
        }
        return md5.toString();
    }
};

#endif
