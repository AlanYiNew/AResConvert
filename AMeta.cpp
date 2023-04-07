#include "AMeta.h"
#include "AEndiannessHelper.h"
#include <stdexcept>
#include <iostream>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>

void AFileMeta::CreateMessageMeta(const AMessageMeta& meta)
{
    m_message_meta.emplace(meta.GetName(), meta);
}

void AFileMeta::CreateEnumMeta(const AEnumMeta& meta)
{
    m_enum_meta.emplace(meta.GetName(), meta);
}

const AMessageMeta* AFileMeta::GetMessageMetaByName(const std::string& name)  const
{
    auto iter = m_message_meta.find(name);
    if (iter == m_message_meta.end()) {
        return nullptr;
    }
    return &iter->second;
}

AFileMeta::AFileMeta(const std::string& name):m_name(name){}

AFileMeta::AFileMeta(unsigned char* buffer, int size) {
    int32_t total_size = 0;
    int32_t name_size = 0;
    buffer += ReadInt32(buffer, total_size);
    buffer += ReadInt32(buffer, name_size);
    buffer += ReadString(buffer, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , 1actual:"+std::to_string(m_name.size()));
    }

    int32_t message_meta_size;
    int32_t cum_size = 0;
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
            throw std::runtime_error("1inconsistent serialize_size:" + std::to_string(s_size) + ", size:" + std::to_string(size));
        }
        m_message_meta.emplace(message_meta.GetName(), message_meta);
        
        cum_size += size;
    }

    if (cum_size != message_meta_size) {
        throw std::runtime_error("2inconssistent serialize_size:" + std::to_string(cum_size) + ", size:" + std::to_string(message_meta_size));
    }

    buffer += cum_size;
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
            throw std::runtime_error("3inconsistent serialize_size:" + std::to_string(s_size) + ", size:" + std::to_string(size));
        }
        m_enum_meta.emplace(enum_meta.GetName(), enum_meta);
        cum_size += size;
    }

    if (cum_size != enum_meta_size) {
        throw std::runtime_error("4inconssistent serialize_size:" + std::to_string(cum_size) + ", size:" + std::to_string(enum_meta_size));
    }
}

int32_t AFileMeta::GetMessageMetaSerializationSize() const {
    int32_t size = 0;
    for (auto& iter: m_message_meta) {
        auto& meta = iter.second;
        size += 4 + meta.GetSerializationSize();
    }
    return size;
}

int32_t AFileMeta::GetEnumMetaSerializationSize() const {
    int32_t size = 0;
    for (auto& iter: m_enum_meta) {
        auto& meta = iter.second;
        size += 4 + meta.GetSerializationSize();
    }
    return size;
}

bool AFileMeta::Serialize(std::vector<unsigned char>& buffer,  int32_t offset) const {
    int32_t message_meta_size = GetMessageMetaSerializationSize();
    int32_t enum_meta_size = GetEnumMetaSerializationSize();
    int32_t name_size = m_name.size();
    int32_t total_size = GetSerializationSize();
    buffer = std::vector<unsigned char>(total_size, 0);

    GOOGLE_LOG(INFO) << "total:" << total_size ;
    offset += WriteInt32(buffer.data()+offset, total_size);
    offset += WriteInt32(buffer.data()+offset, name_size);
    offset += WriteString(buffer.data()+offset, m_name);
    
    offset += WriteInt32(buffer.data()+offset, message_meta_size);
    for (auto& iter: m_message_meta) {
        auto& meta = iter.second;
        int32_t size = meta.GetSerializationSize();
        offset += WriteInt32(buffer.data()+offset, size);
        GOOGLE_LOG(INFO) << "name:" << meta.GetName() << ", offset:" << offset <<  ", size:" << size ;
        meta.Serialize(buffer, offset);
        offset += size;
    }
    GOOGLE_LOG(INFO) << "enum_meta_offset" << offset ;
    offset += WriteInt32(buffer.data()+offset, enum_meta_size);
    for (auto& iter: m_enum_meta) {
        auto& meta = iter.second;
        int32_t size = meta.GetSerializationSize();
        offset += WriteInt32(buffer.data()+offset, size);
        meta.Serialize(buffer, offset);
        offset += size;
    }
    return true;
}

int32_t AFileMeta::GetSerializationSize() const{
    int32_t message_meta_size = GetMessageMetaSerializationSize();
    int32_t enum_meta_size = GetEnumMetaSerializationSize();
    int32_t name_size = m_name.size();
    return message_meta_size + enum_meta_size + m_name.size() + 1 + sizeof(message_meta_size) + sizeof(enum_meta_size) + sizeof(name_size) + 4;
}

AMessageMeta::AMessageMeta(const std::string& name): m_name(name) {};

std::string  AMessageMeta::GetMD5() const {
    std::vector<unsigned char > vec(GetSerializationSize(), 0);
    Serialize(vec);
    MD5 md5;
    md5.update((void*)vec.data(), vec.size());
    return md5.toString();
}

const std::string& AMessageMeta::GetName() const{
    return m_name;
}

AMessageMeta::AMessageMeta(unsigned char* buffer, int32_t buffer_size) 
{
    int32_t cum_size = 0;
    int32_t name_size = 0;
    cum_size += ReadInt32(buffer + cum_size, name_size);
    cum_size += ReadString(buffer + cum_size, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , 2actual:"+std::to_string(m_name.size()));
    }
    while (cum_size < buffer_size) {
        int32_t cur_size;
        cum_size += ReadInt32(buffer+cum_size, cur_size);
        AFieldMeta field_meta(buffer+cum_size, cur_size);
        meta_data[field_meta.name] = field_meta; 
        cum_size+=cur_size;
    }
}

void AMessageMeta::CreateFieldMeta(const AFieldMeta& field_meta) {
    meta_data[field_meta.name] = field_meta;
}

bool AMessageMeta::Serialize(std::vector<unsigned char>& buffer, int32_t offset) const {
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

int32_t AMessageMeta::GetSerializationSize() const {
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

AFieldMeta::AFieldMeta(const std::string& field, FIELDTYPE f_type, const std::string& type_name, int32_t size, int32_t offset, int32_t count, bool repeated)
:type_name(type_name), name(field), size(size), offset(offset), count(count), field_type(f_type), repeated(repeated) {
}

AFieldMeta::AFieldMeta(unsigned char* buffer, int32_t buffer_size) {
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

bool AFieldMeta::Serialize(std::vector<unsigned char>& buffer, int32_t buff_offset) const {
    buff_offset += WriteInt32(buffer.data() + buff_offset, size);
    buff_offset += WriteInt32(buffer.data() + buff_offset, offset);
    buff_offset += WriteInt32(buffer.data() + buff_offset, count);
    buff_offset += WriteString(buffer.data() + buff_offset, name);
    buff_offset += WriteString(buffer.data() + buff_offset, type_name);
    return true;
}

int32_t AFieldMeta::GetSerializationSize() const {
    return name.size() + 1 + sizeof(size) + sizeof(offset) + sizeof(count) + type_name.size() + 1;
}

bool AEnumMeta::Serialize(std::vector<unsigned char>& buffer, int32_t offset) const {
    GOOGLE_LOG(INFO) << "enum_name_offset:" << offset ;
    offset += WriteInt32(buffer.data() + offset, m_name.size());
    offset += WriteString(buffer.data() + offset, m_name);
    for (auto& iter : meta_data) {
        auto& enum_field_meta = iter.second; 
        int32_t size = enum_field_meta.GetSerializationSize();
        GOOGLE_LOG(INFO) << "enum_field_offset:" << offset ;
        offset += WriteInt32(buffer.data() + offset, size);
        if (!enum_field_meta.Serialize(buffer, offset)) {
            return false;   
        };
        offset += size;
   } 
   return true;
}

int32_t AEnumMeta::GetSerializationSize() const {
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

AEnumMeta::AEnumMeta(unsigned char* buffer, int32_t buffer_size) {
    int32_t cum_size = 0;
    int32_t name_size = 0;
    cum_size += ReadInt32(buffer + cum_size, name_size);
    cum_size += ReadString(buffer + cum_size, m_name);
    if (m_name.size() != name_size) {
        throw std::runtime_error("name size inconsistent record:" + std::to_string(name_size) +" , 3actual:"+std::to_string(m_name.size()));
    }
    while (cum_size < buffer_size) {
        int32_t cur_size = 0;
        cum_size += ReadInt32(buffer+cum_size, cur_size);
        AEnumFieldMeta field_meta(buffer+cum_size, cur_size);
        meta_data[field_meta.name] = field_meta; 
        cum_size+=cur_size;
    }
}

AEnumFieldMeta::AEnumFieldMeta(unsigned char* buffer, int32_t buffer_size) {
    buffer += ReadInt32(buffer, value);
    buffer += ReadString(buffer, name);
}

bool AEnumFieldMeta::Serialize(std::vector<unsigned char>& buffer, int32_t buff_offset) const {
    buff_offset += WriteInt32(buffer.data() + buff_offset, value);
    buff_offset += WriteString(buffer.data() + buff_offset, name);
    return true;
}

int32_t AEnumFieldMeta::GetSerializationSize() const{
    return name.size() + 1 + sizeof(value);
}
