#include <PingService.h>
#include "nlohmann/json.hpp"

bool LoadPingFooRequest(PingFooRequest& to, const json& from) 
{
    to.id = from.Get<std::string>(); //1
}
bool LoadPingFooResponse(PingFooResponse& to, const json& from) 
{
    to.fooName = from.Get<std::string>(); //1
    to.fooValue = from.Get<int64_t>(); //2
    LoadPingFooRequest(to.msg, from[msg]);
}
