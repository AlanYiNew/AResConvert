#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include "aresconvert_generator.h"
#include "webview.h"

std::filesystem::path cwd = std::filesystem::current_path() / "filename.txt";

struct Application {
    AResConvertGenerator aresconvert_generator;
    
    json Convert(const json& req) {
        std::cout << "[Convert]: " << req.dump()  << std::endl;
        json result;
        result["message"] = "test";
        return result;
    }

    json GetResourceNameAll(const json& req) {
        json result;
        result["result"] = aresconvert_generator.GetResourceNameAll();
        std::cout << "[GetResourceNameAll]: Req=" << req.dump()  << std::endl;
        std::cout << "[GetResourceNameAll]: Rsp=" << result.dump()  << std::endl;
        return result; 
    }
};

template <typename Func, typename Obj>
auto HandlerWrapper(Func f, Obj* obj)
{
    auto handler = [=](const std::string &s) -> std::string {
        try
        {
            json req = json::parse(s);
            json rsp = (obj->*f)(std::forward<json>(req));
            return rsp.dump();
        }
        catch (json::parse_error& ex)
        {
            std::string message = "parse error at byte " + ex.byte;
            std::cerr << message << std::endl;
            json result;
            result["message"] = message;
            return result;
        }
    };
    return handler;
}

int main(int argc,const char* argv[]) {
    google::protobuf::compiler::CommandLineInterface cli;

    Application app;
    // 生成资源列表 
    cli.RegisterGenerator("--aresconvert_out", "--aresconvert_opt", &app.aresconvert_generator, "Generate AResConvert source and header.");
    argv[argc++] = "-I.";
    cli.Run(argc, argv);

    // 展示界面
    std::string path = "./Client/dist/index.html";
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::filesystem::path cwd = std::filesystem::current_path();

    std::string content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    webview::webview w(true, nullptr, 1024, 768);
    w.bind("Convert", HandlerWrapper(&Application::Convert, &app));
    w.bind("GetResourceNameAll", HandlerWrapper(&Application::GetResourceNameAll, &app));

    w.navigate("https://appassets.example/index.html");
    w.run();
    return 0;    
}

