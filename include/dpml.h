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

    void InitAll();

private:
    void InitRadarData()
    {
        mData = make_shared<RadarData>(mDpmlConf);
    }

    void InitDpmlConf(string conf_file)
    {
        mDpmlConf = make_shared<DpmlConf>(conf_file);
        Path::SetRadarParams(mDpmlConf->GetRadarParams());
    }

    void InitPath()
    {
        for(int i = 0;i<n_radar;++i){
            auto path = make_shared<Path>(n_radar,i,0,mData->toa()[0]);
            mPaths.push_back(path);
        }
        return;
    }
private:
    DpmlConfPtr mDpmlConf;
    RadarDataPtr mData;

    int n_radar;
    vector<PathPtr> mPaths;
};


void update_likelihood(int k,double pt,Path& P);

//计算真实路径的似然值
double true_path_ll();

#endif