#include <iostream>
#include <fstream>
#include <vector>
#include "helpers.h"
#include "application.h"
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>

#ifdef _WIN32
#include "webview.h"
#endif

Application app;

template <typename Func, typename Obj>
auto HandlerWrapper(Func f, Obj* obj)
{
    
    auto handler = [=](const std::string &s) -> std::string {
        auto& log_stream = app.GetLogStream();
        auto old_cerr = std::cerr.rdbuf();
        auto old_cout = std::cout.rdbuf();
        std::cerr.rdbuf(log_stream.rdbuf());
        std::cout.rdbuf(log_stream.rdbuf());
        json result;
        try
        {
            json req = json::parse(s);
            result = (obj->*f)(std::forward<json>(req));
        }
        catch (json::parse_error& ex)
        {
            result["code"] = -1;
            GOOGLE_LOG(ERROR) << "parse error at byte " + ex.byte;
        }
        std::cout.rdbuf(old_cout);
        std::cerr.rdbuf(old_cerr);

        // LogHandling
        const std::string& logs = log_stream.str();
        if (logs.size() != 0) {
            std::vector<std::string> out = stringSplit(logs, "\n");
            for (auto& x: out){
                result["log"].push_back(x);
            }
        }
        log_stream.str("");

        return result.dump();

    };
    return handler;
}

void CustomLogHandler(LogLevel level, const char* filename, int line, const std::string& message) {
    const char* level_name = nullptr;
    switch (level) {
        case LOGLEVEL_ERROR:
            level_name = "error"; break;
        case LOGLEVEL_FATAL:
            level_name = "fatal"; break;
        case LOGLEVEL_WARNING:
            level_name = "warning"; break;
        default:
            level_name = "info"; break;
    }

    time_t t = time(nullptr);
    std::string timeStr = std::string(std::ctime(&t));
    timeStr = "[" + timeStr.substr(0, timeStr.size()-1) + "] ";
    std::string log = timeStr + filename +  ":" + std::to_string(line) + ": " + level_name + ": " + message;
    if (level > LOGLEVEL_INFO) {
        std::cerr << log;
        printf("%s", log.c_str());
    }
    else {
        std::cout << log;
        printf("%s", log.c_str());
    }
}

int main(int argc, char* argv[]) {
#ifdef _WIN32 
    SetLogHandler(CustomLogHandler);
#endif
    if (app.LoadSetting("setup.json", argc, argv)) {
        app.Run();
    }

#ifdef _WIN32
    webview::webview w(true, nullptr, 1024, 768);
    w.bind("Convert", HandlerWrapper(&Application::Convert, &app));
    w.bind("GetResourceNameAll", HandlerWrapper(&Application::GetResourceNameAll, &app));
    w.bind("GetErrMessage", HandlerWrapper(&Application::GetErrMessage, &app));
    w.bind("Refresh", HandlerWrapper(&Application::Refresh, &app));
    w.navigate("https://appassets.example/index.html");
    w.run();
#endif
    return 0;    
}

