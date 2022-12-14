#include "AMeta.h"
#include "AEndianessHelper.h"
#include <stdexcept>
#include <iostream>

void AFileMeta::CreateMessageMeta(const AMessageMeta& meta)
{
    message_meta.emplace(meta.GetName(), meta);
}

void AFileMeta::CreateEnumMeta(const AEnumMeta& meta)
{
    enum_meta.emplace(meta.GetName(), meta);
}

AFileMeta::AFileMeta(const std::string& name):m_name(name){}

AFileMeta::AFileMeta(char* buffer, int size) {
    int32_t cum_size = 0;
    int32_t total_size = 0;
    int32_t name_size = 0;
    cum_size += ReadInt32(buffer + cum_size, total_size);
    cum_size += ReadInt32(buffer + cum_size, name_size);
    cum_size += ReadString(buffer + cum_size, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , actual:"+std::to_string(m_name.size()));
    }

    int32_t message_meta_size;
    buffer += ReadInt32(buffer, message_meta_size);
    while (cum_size < message_meta_size) {
        if (cum_size < 0) {
            throw std::runtime_error("invalid size read " + std::to_string(message_meta_size));
        }

        int32_t size = 0;
        cum_size += ReadInt32(buffer + cum_size, size);
        AMessageMeta message_meta(buffer + cum_size, size);
        int32_t s_size = message_meta.GetSerializationSize();
        if (s_size != size) {
            throw std::runtime_error("inconsistent serialize_size:" + std::to_string(s_size) + ", size:" + std::to_string(size));
        }
        cum_size += size;
    }

    if (cum_size != message_meta_size) {
        throw std::runtime_error("inconssistent serialize_size:" + std::to_string(cum_size) + ", size:" + std::to_string(message_meta_size));
    }

    int32_t enum_meta_size = 0;
    buffer += ReadInt32(buffer, enum_meta_size);
    cum_size = 0;
    while (cum_size < enum_meta_size) {
        if (cum_size < 0) {
            throw std::runtime_error("invalid size read " + std::to_string(enum_meta_size));
        }

        int32_t size = 0;
        cum_size += ReadInt32(buffer + cum_size, size);
        AEnumMeta enum_meta(buffer + cum_size, size);
        int32_t s_size = enum_meta.GetSerializationSize();
        if (s_size != size) {
            throw std::runtime_error("inconsistent serialize_size:" + std::to_string(s_size) + ", size:" + std::to_string(size));
        }
        cum_size += size;
    }

    if (cum_size != enum_meta_size) {
        throw std::runtime_error("inconssistent serialize_size:" + std::to_string(cum_size) + ", size:" + std::to_string(enum_meta_size));
    }
}

int32_t AFileMeta::GetMessageMetaSerializationSize() {
    int32_t size = 0;
    for (auto& iter: message_meta) {
        auto& meta = iter.second;
        size += 4 + meta.GetSerializationSize();
    }
    return size;
}

int32_t AFileMeta::GetEnumMetaSerializationSize() {
    int32_t size = 0;
    for (auto& iter: enum_meta) {
        auto& meta = iter.second;
        size += 4 + meta.GetSerializationSize();
    }
    return size;
}

bool AFileMeta::Serialize(std::vector<unsigned char>& buffer,  int32_t offset) {
    int32_t message_meta_size = GetMessageMetaSerializationSize();
    int32_t enum_meta_size = GetEnumMetaSerializationSize();
    int32_t name_size = m_name.size();
    int32_t total_size = GetSerializationSize();
    buffer = std::vector<unsigned char>(total_size, 0);
    
    offset += WriteInt32(buffer.data()+offset, message_meta_size);
    offset += WriteInt32(buffer.data()+offset, name_size);
    offset += WriteString(buffer.data()+offset, m_name);
    for (auto& iter: message_meta) {
        auto& meta = iter.second;
        int32_t size = meta.GetSerializationSize();
        offset += WriteInt32(buffer.data()+offset, size);
        meta.Serialize(buffer, offset);
        offset += size;
    }
    offset += WriteInt32(buffer.data()+offset, enum_meta_size);
    for (auto& iter: enum_meta) {
        auto& meta = iter.second;
        int32_t size = meta.GetSerializationSize();
        offset += WriteInt32(buffer.data()+offset, size);
        meta.Serialize(buffer, offset);
        offset += size;
    }
    return true;
}
int32_t AFileMeta::GetSerializationSize() {
    int32_t message_meta_size = GetMessageMetaSerializationSize();
    int32_t enum_meta_size = GetEnumMetaSerializationSize();
    int32_t name_size = m_name.size();
    return message_meta_size + enum_meta_size + m_name.size() + sizeof(message_meta_size) + sizeof(enum_meta_size) + sizeof(name_size);
}

AMessageMeta::AMessageMeta(const std::string& name): m_name(name) {};

const std::string& AMessageMeta::GetName() const{
    return m_name;
}

AMessageMeta::AMessageMeta(char* buffer, int32_t buffer_size) 
{
    int32_t cum_size = 0;
    int32_t name_size = 0;
    cum_size += ReadInt32(buffer + cum_size, name_size);
    cum_size += ReadString(buffer + cum_size, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , actual:"+std::to_string(m_name.size()));
    }
    while (cum_size < buffer_size) {
        int32_t cur_size;
        cum_size += ReadInt32(buffer, cur_size);
        AFieldMeta field_meta(buffer+cum_size, cur_size);
        meta_data[field_meta.name] = field_meta; 
        cum_size+=cur_size;
    }
}

void AMessageMeta::CreateFieldMeta(const AFieldMeta& field_meta) {
    meta_data[field_meta.name] = field_meta;
}

bool AMessageMeta::Serialize(std::vector<unsigned char>& buffer, int32_t offset) {
    offset += WriteInt32(buffer.data() + offset, m_name.size());
    offset += WriteString(buffer.data() + offset, m_name);
    for (auto& iter : meta_data) {
        auto& field_meta = iter.second;
        int32_t size = field_meta.GetSerializationSize();
        offset += WriteInt32(buffer.data()+offset, size);
        if (!field_meta.Serialize(buffer, offset)) {
            return false;   
        };
        offset += size ;
    } 
    return true;
}

int32_t AMessageMeta::GetSerializationSize() {
    int32_t size = 0;
    for (auto& iter : meta_data) {
        auto& field_meta = iter.second;
        size += field_meta.GetSerializationSize() + sizeof(size);
    }
    int32_t name_size = m_name.size();
    return sizeof(name_size) + m_name.size() + 1 + size;
}

const std::string& AEnumMeta::GetName() const {
    return m_name;
}

AFieldMeta::AFieldMeta(const std::string& field, const std::string& type_name, FIELDTYPE field_type, int32_t size, int32_t offset, int32_t count)
:type_name(type_name), field_type(field_type), name(field), size(size), offset(offset), count(count) {
}

AFieldMeta::AFieldMeta(char* buffer, int32_t buffer_size) {
    buffer += ReadInt32(buffer, size);
    buffer += ReadInt32(buffer, offset);
    buffer += ReadInt32(buffer, count);
    buffer += ReadString(buffer, name);
    buffer += ReadString(buffer, type_name);

    int32_t calc_size = GetSerializationSize();
    if (calc_size != buffer_size) {
        throw std::runtime_error("Serialization buffer size not consistent " + std::to_string(calc_size) + " " + std::to_string(buffer_size));
    }
}

bool AFieldMeta::Serialize(std::vector<unsigned char>& buffer, int32_t buff_offset) {
    buff_offset += WriteInt32(buffer.data() + buff_offset, size);
    buff_offset += WriteInt32(buffer.data() + buff_offset, offset);
    buff_offset += WriteInt32(buffer.data() + buff_offset, count);
    buff_offset += WriteString(buffer.data() + buff_offset, name);
    buff_offset += WriteString(buffer.data() + buff_offset, type_name);
    return true;
}

int32_t AFieldMeta::GetSerializationSize() {
    return name.size() + 1 + sizeof(size) + sizeof(offset) + sizeof(field_type) + type_name.size() + 1;
}

bool AEnumMeta::Serialize(std::vector<unsigned char>& buffer, int32_t offset) {
    offset += WriteInt32(buffer.data() + offset, m_name.size());
    offset += WriteString(buffer.data() + offset, m_name);
    for (auto& iter : meta_data) {
        auto& enum_field_meta = iter.second; 
        int32_t size = enum_field_meta.GetSerializationSize();
        offset += WriteInt32(buffer.data() + offset, size);
        if (!enum_field_meta.Serialize(buffer, offset)) {
            return false;   
        };
        offset += size;
   } 
   return true;
}

int32_t AEnumMeta::GetSerializationSize() {
    int32_t size = 0;
    for (auto& iter: meta_data) {
        auto& enum_field_meta = iter.second; 
        size += enum_field_meta.GetSerializationSize() + sizeof(size);
    }
    int32_t name_size = m_name.size();
    return sizeof(name_size) + name_size + 1 + size;
}


void AEnumMeta::CreateEnumFieldMeta(const AEnumFieldMeta& enum_field_meta) {
    meta_data[enum_field_meta.name] = enum_field_meta;
}

AEnumMeta::AEnumMeta(const std::string& name): m_name(name) {}

AEnumMeta::AEnumMeta(char* buffer, int32_t buffer_size) {
    int32_t cum_size = 0;
    int32_t name_size = 0;
    cum_size += ReadInt32(buffer + cum_size, name_size);
    cum_size += ReadString(buffer + cum_size, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , actual:"+std::to_string(m_name.size()));
    }
    while (cum_size < buffer_size) {
        int32_t cur_size = 0;
        cum_size += ReadInt32(buffer, cur_size);
        AEnumFieldMeta field_meta(buffer+cum_size, cur_size);
        meta_data[field_meta.name] = field_meta; 
        cum_size+=cur_size;
    }
}

AEnumFieldMeta::AEnumFieldMeta(char* buffer, int32_t buffer_size) {
    buffer += ReadInt32(buffer, value);
    buffer += ReadString(buffer, name);
}

bool AEnumFieldMeta::Serialize(std::vector<unsigned char>& buffer, int32_t buff_offset) {
    buff_offset += WriteInt32(buffer.data() + buff_offset, value);
    buff_offset += WriteString(buffer.data() + buff_offset, name);
    return true;
}

int32_t AEnumFieldMeta::GetSerializationSize() {
    return name.size() + 1 + sizeof(value);
}
