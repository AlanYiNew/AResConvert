#include "AMeta.h"
#include "PingService.h"
#include <iostream>
#include "Test.h"
#include "AMeta.h"
#include "AEndiannessHelper.h"

template<typename T>
bool LoadFile(const AFileMeta& file_meta, const std::string& file_name, const std::string& message_name, void* buffer, int buffer_size) {
    std::FILE* file = fopen(file_name.c_str(), "r");
    if (!file) return -1;

    const AMessageMeta* message_meta = file_meta.GetMessageMetaByName(message_name);
    if (message_meta == nullptr) {
        std::cerr << "Fail to find message_meta" << std::endl;
        return false;
    }

    AResourceHead header;
    if (!ReadInt32(file, header.total_size)) {
        std::cerr << "Fail to read total_size" << std::endl;
        return false;
    }

    if (!ReadInt32(file, header.size)) {
        std::cerr << "Fail to read size" << std::endl;
        return false;
    }

    if (!ReadInt32(file, header.count)) {
        std::cerr << "Fail to read count" << std::endl;
        return false;
    }

    if (!ReadBytes(file, header.meta_md5, sizeof(header.meta_md5))) {
        std::cerr << "Fail to read meta_md5" << std::endl;
        return false;
    }

    if (!ReadBytes(file, header.content_md5, sizeof(header.content_md5))) {
        std::cerr << "Fail to read content_md5" << std::endl;
        return false;
    }

    std::string res_meta_md5 = std::string((char*)header.meta_md5, sizeof(header.meta_md5));
    std::string meta_md5 = message_meta->GetMD5();
    if (res_meta_md5 != meta_md5) {
        std::cerr << "Faill to compare md5 " << meta_md5 << "|" << res_meta_md5;
        return false;
    }

    if (header.size != sizeof(T)) {
        std::cerr << header.size << "!=" << sizeof(T) << std::endl;
        return false;
    }
    
    if (header.count * header.size > buffer_size) {
        std::cerr << "buffer too smalle" << std::endl;
        return false;
    }

    std::fseek(file, sizeof(AResourceHead), SEEK_SET);
    auto x = std::fread(buffer, sizeof(PingFooResponse), header.count, file);
    if (x != header.count * header.size) {
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    auto buffer = new PingFooResponse[41000];
    AFileMeta file_meta(gs_PingServiceMeta, sizeof(gs_PingServiceMeta));
    LoadFile<PingFooResponse>(file_meta, "PingFooResponse", "../bin/ping_foo_response_cs.bin", buffer, sizeof(PingFooResponse) * 41000);

    for (int i = 0; i < 20; ++i) {
        std::cout << buffer[i].fooName << "|" << buffer[i].fooValue
        << "|" << buffer[i].msg[0].id << "|" << buffer[i].msg[1].id
        << "|" << buffer[i].msgp[0].id << "|" << buffer[i].msgp[1].id << std::endl;
    }

    return 0;
}
