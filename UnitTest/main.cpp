#include "AMeta.h"
#include "PingService.h"
#include <iostream>
#include "Test.h"

int main(int argc, char* argv[]) {
    std::FILE* file = fopen("ping_foo_response_cs.bin", "r");
    if (!file) return -1;

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
