#ifndef ARESCONVERT_PINGSERVICE_
#define ARESCONVERT_PINGSERVICE_

struct PingFooRequest {
    std::string id; //1
}

struct PingFooResponse {
    std::string fooName; //1
    int64_t fooValue; //2
    PingFooRequest msg; //3
}


#endif
