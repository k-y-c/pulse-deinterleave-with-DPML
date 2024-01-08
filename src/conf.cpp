#include "conf.h"
#include "nanolog.hpp"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

DpmlConf::DpmlConf(string conf_file)
{
    ifstream ifs(conf_file);
    if(!ifs.good())
    {
        LOG_CRIT << "FileNotFound." << conf_file.c_str();
        cerr << "FileNotFound." << conf_file.c_str();
    }
    ifs >> json::wrap(mDpmlConf);
}

