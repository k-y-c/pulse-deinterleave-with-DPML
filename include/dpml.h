#ifndef DPML
#define DPML

#include <bits/stdc++.h>
#include "conf.h"
#include "data.h"
#include "path.h"
using namespace std;

// TODO: 建一个DPML类，统筹算法
class DpmlAlgrithm
{
public:
    DpmlAlgrithm();

    void start();

    void SetConf(const string& conf_file);

    void InitAll();

private:
    void InitRadarData();

    void InitDpmlConf(const string& conf_file);

    void InitPath();

private:
    string mConfigFile;
    DpmlConfPtr mDpmlConf;
    RadarDataPtr mData;

    int n_radar;
    int buffer_size;
    vector<Path> mPaths;
};


void update_likelihood(int k,double pt,Path& P);

//计算真实路径的似然值
double true_path_ll();

#endif