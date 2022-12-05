#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include "aresconvert_generator.h"
#include "webview.h"
#include "helpers.h"
#include "OpenXLSX.hpp"


using namespace OpenXLSX;

struct Settings {
    std::vector<std::string> files;
    std::vector<std::string> folders;
};

struct Application {
    Settings conf;
    char* program_name;
    std::vector<std::string> err_message;
    std::vector<std::unique_ptr<char[]>> plugin_arguments;
    AResConvertGenerator aresconvert_generator;

    bool LoadSetting(const std::string& path, char* program_name) {
        this->program_name = program_name;
        json settings;
        std::ifstream jfile(path);
        if (!jfile.is_open()) {
            GOOGLE_LOG(ERROR) << "Could not open the file";
            return false;
        }

        try {
            jfile >> settings;
        }   catch (json::exception& e) {
            GOOGLE_LOG(ERROR) << "Fail parsing files: " <<  path <<  ", " <<  e.what(); 
            return false;
        }

        try {
            conf.files = settings["files"].get<std::vector<std::string>>();
        }   catch (json::exception& e) {
            GOOGLE_LOG(ERROR) << "When loading param files. " << e.what(); 
            return false;
        }

        try {
            conf.folders = settings["folders"].get<std::vector<std::string>>();
        }   catch (json::exception& e) {
            GOOGLE_LOG(ERROR) <<  "When loading param folders. " << e.what(); 
            return false;
        }
   
        return true;
    }

    void Run() {

        std::vector<std::string> args = {
            "--aresconvert_out=protocol",
            "--aresconvert_opt=resource", 
            "-I=./",
        };

        const int argument_option_len = 128;
        for (auto& str: args) {
            auto len = str.size() + argument_option_len;
            plugin_arguments.push_back(std::make_unique<char[]>(len));
            snprintf(plugin_arguments.back().get(), len, "%s", str.c_str());
        }

        for (auto& str: conf.folders) {
            auto len = str.size() + argument_option_len;
            plugin_arguments.push_back(std::make_unique<char[]>(len));
            snprintf(plugin_arguments.back().get(), len, "-I=%s", str.c_str());
        }   

        for (auto& str: conf.files) {
            auto len = str.size() + argument_option_len;
            plugin_arguments.push_back(std::make_unique<char[]>(len));
            snprintf(plugin_arguments.back().get(), len, "%s", str.c_str());
        }

        auto size = plugin_arguments.size();
        char** argv = new char*[size + 1];
        int argc = 0;
        argv[argc++] = program_name;
        for (auto& cstr : plugin_arguments) {
            argv[argc++] = cstr.get();
        }

        // 生成资源列表 
        google::protobuf::compiler::CommandLineInterface cli;
        cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", &aresconvert_generator, "Generate AResConvert source and header.");
        cli.Run(argc, argv);
        delete argv;
    }
    
    json Convert(const json& req) {
        std::string res_name = req[0]["res_name"].get<std::string>();
        sprintf(plugin_arguments[1].get(), "--aresconvert_opt=convert:%s", res_name.c_str());

        auto size = plugin_arguments.size();
        char** argv = new char*[size + 1];
        int argc = 0;
        argv[argc++] = program_name;
        for (auto& cstr : plugin_arguments) {
            argv[argc++] = cstr.get();
        }

        // 生成资源列表 
        google::protobuf::compiler::CommandLineInterface cli;
        cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", &aresconvert_generator, "Generate AResConvert source and header.");
        cli.Run(argc, argv);

        std::string xlsx_name = aresconvert_generator.FindXLSXFileNameByMessageName(res_name);
        std::cout << xlsx_name << std::endl;
        XLDocument doc;
        doc.create("Spreadsheet.xlsx");
        auto wks = doc.workbook().worksheet("Sheet1");

        wks.cell("A1").value() = "Hello, OpenXLSX!";

        doc.save();

        json result;
        result["message"] = "success";
        return result;
    }

    json GetErrMessage(const json& req) {
        json result;
        return result;
    }

    json GetResourceNameAll(const json& req) {
        json result;
        result["result"] = aresconvert_generator.GetResourceNameAll();
        return result; 
    }
};

Application app;

template <typename Func, typename Obj>
auto HandlerWrapper(Func f, Obj* obj)
{
    
    auto handler = [=](const std::string &s) -> std::string {
        std::stringstream sstream;
        auto old = std::cerr.rdbuf(sstream.rdbuf());
        json result;
        try
        {
            json req = json::parse(s);
            result = (obj->*f)(std::forward<json>(req));
            if (!result.contains("code")) {
                result["code"] = 0;
            }
        }
        catch (json::parse_error& ex)
        {
            result["code"] = -1;
            std::string message = "parse error at byte " + ex.byte;
            GOOGLE_LOG(ERROR) << message;
        }

        // LogHandling
        const std::string& logs = sstream.str();
        if (logs.size() != 0) {
            result["code"] = -1;
            std::vector<std::string> out = stringSplit(logs, "\n");
            for (auto& x: out){
                result["log"].push_back(x);
            }
        }
        for (auto &x: obj->err_message) {
            result["log"].push_back(x);
        }
        obj->err_message.clear();
        std::cerr.rdbuf(old);
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

    std::string log = std::string(filename) +  ":" + std::to_string(line) + ": " + level_name + ": " + message;
    if (level > LOGLEVEL_INFO) {
        app.err_message.push_back(log);
        std::cout << log << std::endl;
    }
    else {
        std::cout << log << std::endl; 
    }
}

int main(int argc, char* argv[]) {
    SetLogHandler(CustomLogHandler);
    if (app.LoadSetting("setup.json", argv[0])) {
        app.Run();
    }

    webview::webview w(true, nullptr, 1024, 768);
    w.bind("Convert", HandlerWrapper(&Application::Convert, &app));
    w.bind("GetResourceNameAll", HandlerWrapper(&Application::GetResourceNameAll, &app));
    w.bind("GetErrMessage", HandlerWrapper(&Application::GetErrMessage, &app));

    w.navigate("https://appassets.example/index.html");
    w.run();
    return 0;    
}

