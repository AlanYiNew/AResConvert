#include <google/protobuf/compiler/plugin.h>
#include <iostream>
#include "arpc_generator.h"
#include <google/protobuf/compiler/cpp/generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
  
int main(int argc, char* argv[]) {
    //CGenerator generator;
    //google::protobuf::compiler::PluginMain(
    //   argc, argv, &generator);
    google::protobuf::compiler::CommandLineInterface cli;

    ARPCGenerator arpc_generator;
    // 官方实现的C++编译器后端
    cli.RegisterGenerator("--arpc_out", &arpc_generator, "Generate ARPC source and header.");
    /*
    argc = 6;
    argv[1] = "--c_out=./";
    argv[2] = "-IC:\\Users\\guoha\\source\\repos\\AConvert\\include";
    argv[3] = "-IC:\\Users\\guoha\\source\\repos\\AConvert\\proto";
    argv[4] = "-IC:\\Users\\guoha\\source\\repos\\AConvert";
    argv[5] = "C:\\Users\\guoha\\source\\repos\\AConvert\\AResource.proto";
    
    cli.RegisterGenerator("--c_out", &cgenerator,
        "Generate Foo file.");
    argc = 6;
    argv[1] = "--c_out=./";
    argv[2] = "-IC:\\Users\\guoha\\source\\repos\\AConvert\\AConvert\\include";
    argv[3] = "-IC:\\Users\\guoha\\source\\repos\\AConvert\\AConvert\\proto";
    argv[4] = "-IC:\\Users\\guoha\\source\\repos\\AConvert\\AConvert";
    argv[5] = "C:\\Users\\guoha\\source\\repos\\AConvert\\AConvert\\AResource.proto";
    */
    //std::cout << argv[2] << std:: endl;
    return cli.Run(argc, argv);
    
}

