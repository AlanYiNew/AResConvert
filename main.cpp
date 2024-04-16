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
        std::cerr << log << std::endl;
        printf("%s\n", log.c_str());
    }
    else {
        std::cout << log << std::endl;
        printf("%s\n", log.c_str());
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
    webview::webview w(true, nullptr, 1920, 1080);
    w.bind("Convert", app.HandlerWrapper(&Application::ConvertBin, &w));
    w.bind("ConvertJson", app.HandlerWrapper(&Application::ConvertJson, &w));
    w.bind("GetResourceNameAll", app.HandlerWrapper(&Application::GetResourceNameAll, &w));
    w.bind("GetConf", app.HandlerWrapper(&Application::GetConf, &w));
    w.bind("Refresh", app.HandlerWrapper(&Application::Refresh, &w));
    w.bind("Upload", app.HandlerWrapper(&Application::Upload, &w));
    w.navigate("https://appassets.example/index.html");
    w.run();
#endif
    return 0;    
}

