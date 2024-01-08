#include "dpml.h"

void printUsage(const std::string& programName) {
    std::cerr << "Usage: " << programName << " -c config_file (default:../config/conf.json)" << std::endl;
}

void parse_args(int argc,char* argv[],string & conf_file)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c" && i + 1 < argc) {
            conf_file = argv[++i];
        }
        else
        {
            printUsage(argv[0]);
            exit(1);
        }
    }
}

int main(int argc,char* argv[])
{
    string conf_file;
    parse_args(argc,argv,conf_file);
    DpmlAlgrithm dpml;
    if(conf_file.size()>0)
    {
        dpml.SetConf(conf_file);
    }
    dpml.InitAll();
    dpml.start();
    return 0;
}