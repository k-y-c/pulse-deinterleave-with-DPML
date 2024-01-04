#include "conf.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

DpmlConf::DpmlConf(string conf_file)
{
    ifstream ifs(conf_file);
    ifs >> json::wrap(mDpmlConf);
}

