#include "dpml.h"
#include "util.h"
#include "nanolog.hpp"

int main(int argc,char* argv[])
{
    if(chcwd())
    {
        return 0;
    }
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