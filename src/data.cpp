#include "data.h"
#include <iostream>
#include <fstream>
using namespace std;

RadarData::RadarData(DpmlConfPtr conf):mDpmlConf(conf)
{
    InitData();
}

void RadarData::InitData()
{
    mToa.clear();
    mLabel.clear();
    string toa_file = mDpmlConf->GetDataFile();
    string label_file = mDpmlConf->GetLabelFile();
    fstream fs;
    fs.open(toa_file);
    string s;
    while(fs >> s){
        double val = stod(s);
        mToa.push_back(val);
    }
    fs.close();
    fs.open(label_file);
    while(fs >> s){
        int val = stoi(s);
        mLabel.push_back(val);
    }
}