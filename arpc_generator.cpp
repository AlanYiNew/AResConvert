#include "arpc_generator.h"
#include "inja/inja.hpp"
#include <iostream>

using namespace nlohmann;
bool ARPCGenerator::Generate(const FileDescriptor* file, 
                  const string& parameter,
                  GeneratorContext* generator_context, 
                  string* error) const  
{

    string suffix = ".proto";
    string base_name = StripSuffixString(file->name(), suffix);
    auto temp = generator_context->Open(base_name + ".h");
    std::unique_ptr<io::ZeroCopyOutputStream> output(temp);
    Printer define_printer(temp, '$');
    if (!GenerateDefineFile(file, define_printer)) return false;
    return true;
}


bool ARPCGenerator::GenerateDefineFile(const FileDescriptor* file, Printer& printer) const 
{
    for (int i = 0; i < file->service_count(); i++)
    {
        const ServiceDescriptor* service_descriptor = file->service(i);

        
        json data;
        data["service_name"] = service_descriptor->name();

        inja::Environment env;
        ofstream  afile;
        afile.open(service_descriptor->name() + ".c", ios::out);
        env.render_to(afile, env.parse_template("arpc_service_source.template"), data); // Writes "Hello world!" to stream
    }

    return true;
}
