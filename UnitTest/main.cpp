#include "AMeta.h"
#include "PingService.h"
#include <iostream>
#include "Test.h"
#include "AMeta.h"
#include "AEndiannessHelper.h"

int main(int argc, char* argv[]) {
    std::FILE* file = fopen("../bin/ping_foo_response_cs.bin", "r");
    if (!file) return -1;

    for (int i = 0; i < sizeof(gs_PingServiceMeta); ++i) {
        std::cout << char(gs_PingServiceMeta[i]);
    }
    std::cout << std::endl;

    AFileMeta file_meta(gs_PingServiceMeta, sizeof(gs_PingServiceMeta));
    const AMessageMeta* message_meta = file_meta.GetMessageMetaByName("PingFooResponse");
    if (message_meta == nullptr) {
        std::cerr << "Fail to find message_meta" << std::endl;
        return -1;
    }

    AResourceHead header;
    if (!ReadInt32(file, header.total_size)) {
        std::cerr << "Fail to read total_size" << std::endl;
        return -1;
    }

    if (!ReadInt32(file, header.size)) {
        std::cerr << "Fail to read size" << std::endl;
        return -1;
    }

    if (!ReadInt32(file, header.count)) {
        std::cerr << "Fail to read count" << std::endl;
        return -1;
    }

    if (!ReadBytes(file, header.meta_md5, sizeof(header.meta_md5))) {
        std::cerr << "Fail to read meta_md5" << std::endl;
        return -1;
    }

    if (!ReadBytes(file, header.content_md5, sizeof(header.content_md5))) {
        std::cerr << "Fail to read content_md5" << std::endl;
        return -1;
    }

    std::string res_meta_md5 = std::string((char*)header.meta_md5, sizeof(header.meta_md5));
    std::string meta_md5 = message_meta->GetMD5();
    if (res_meta_md5 != meta_md5) {
        std::cerr << "Faill to compare md5 " << meta_md5 << "|" << res_meta_md5;
        return -1;
    }

    std::fseek(file, sizeof(AResourceHead), SEEK_SET);
    auto buffer = new PingFooResponse[41000];
    std::fread(buffer, sizeof(PingFooResponse), 41000, file);
    for (int i = 0; i < 20; ++i) {
        std::cout << buffer[i].fooName << "|" << buffer[i].fooValue
        << "|" << buffer[i].msg[0].id << "|" << buffer[i].msg[1].id
        << "|" << buffer[i].msgp[0].id << "|" << buffer[i].msgp[1].id << std::endl;
    }

    return 0;
}
