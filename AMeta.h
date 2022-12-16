#ifndef __ARESOURCE_HEAD_
#define __ARESOURCE_HEAD_

#include <cstdint>
#include <cstddef>
#include "md5.h"
#include <unordered_map>
#include <map>
#include <vector>

struct AResourceHead{
    int32_t total_size{};
    int32_t size{}; // size of record in bytes
    int32_t count{}; // number of records
    uint8_t meta_md5[32]{};
    uint8_t content_md5[32]{};
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
    std::string type_name;
    int32_t size;
    int32_t offset;
    int32_t count;
    FIELDTYPE field_type;
    AFieldMeta() =  default;
    AFieldMeta(unsigned char* buffer, int32_t size);
    AFieldMeta(const std::string& field, const std::string& type_name, int32_t size, int32_t offset, int32_t count);
    bool Serialize(std::vector<unsigned char>& buffer, int32_t buff_offset = 0) const;
    int32_t GetSerializationSize() const;
};

struct AEnumFieldMeta {
    std::string name;
    int32_t value;
    AEnumFieldMeta() = default;
    AEnumFieldMeta(unsigned char* buffer, int32_t size);
    AEnumFieldMeta(const std::string& name, int32_t value): name(name), value(value) {};
    bool Serialize(std::vector<unsigned char>& buffer, int32_t offset = 0) const;
    int32_t GetSerializationSize() const;
};

struct AEnumMeta {
    std::map<std::string, AEnumFieldMeta> meta_data;
    std::string m_name;

    AEnumMeta(const std::string& name);
    AEnumMeta(unsigned char* buffer, int32_t size);
    const std::string& GetName() const;
    bool Serialize(std::vector<unsigned char>& buffer, int32_t offset = 0) const;
    int32_t GetSerializationSize() const;
    void CreateEnumFieldMeta(const AEnumFieldMeta& enum_field_meta);
};

class AMessageMeta {
public:
    AMessageMeta(const std::string& name);
    AMessageMeta(unsigned char* buffer, int32_t buffer_size);
    bool Serialize(std::vector<unsigned char>& buffer, int32_t offset = 0) const;
    int32_t GetSerializationSize() const;
    void CreateFieldMeta(const AFieldMeta& field_meta);
    const std::string& GetName() const;
    std::string GetMD5() const;
private:
    std::string m_name;
    //int32_t m_size{}; // size of bytes of the records
    std::map<std::string, AFieldMeta> meta_data;
};

class AFileMeta {
    private:
        std::map<string, AMessageMeta> m_message_meta;
        std::map<string, AEnumMeta> m_enum_meta;
        std::string m_name;
    private:
        int32_t GetMessageMetaSerializationSize() const ; 
        int32_t GetEnumMetaSerializationSize() const;

    public:
        AFileMeta(const std::string& name);
        AFileMeta(unsigned char* buffer, int size);
        void CreateMessageMeta(const AMessageMeta& message_meta);
        void CreateEnumMeta(const AEnumMeta& enum_meta);
        bool Serialize(std::vector<unsigned char>& buffer, int32_t offset = 0) const;
        int32_t GetSerializationSize() const;
        const AMessageMeta* GetMessageMetaByName(const std::string& name);
};

#endif
