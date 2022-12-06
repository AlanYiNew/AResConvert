#ifndef __ARESOURCE_HEAD_
#define __ARESOURCE_HEAD_

#include <cstdint>
#include <cstddef>

struct AResourceHead{
    int32_t total_size{};
    uint8_t md5[16]{};
    int32_t col_num{};
    int32_t col_info_size{};
    int32_t record_size;
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

struct FieldMeta {
    std::string name;
    int32_t size;
    FIELDTYPE field_type;
    FieldMeta(const std::string& field, FIELDTYPE field_type, int size): field_type(field_type), name(field), size(size) {};
};

#endif
