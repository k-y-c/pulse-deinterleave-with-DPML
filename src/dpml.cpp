#include "dpml.h"
#include "conf.h"
#include "nanolog.hpp"
#include <iostream>
using namespace std;


DpmlAlgrithm::DpmlAlgrithm():
mConfigFile("../config/conf.json")
{

}

void DpmlAlgrithm::SetConf(const string& conf_file)
{
    mConfigFile = conf_file;
}

void DpmlAlgrithm::start()
{
    // 按脉冲到达顺序迭代
    for(int pulse_idx = 1;pulse_idx<mData->size();++pulse_idx)
    {
        vector<Path> new_paths;
        // 针对每个路径，为该路径附上可能的标签，并计算ll
        for(Path& p:mPaths)
        {
            for(int label = 0;label<n_radar;++label)
            {
                auto temp_path = p;
                temp_path.update(pulse_idx,label,mData->toa(pulse_idx));
                new_paths.emplace_back(temp_path);
            }
            if(mDpmlConf->IsDealSpurious())
            {
                auto temp_path = p;
                temp_path.update(pulse_idx,SPURIOUS,mData->toa(pulse_idx));
                new_paths.emplace_back(temp_path);
            }
        }

        // 保留似然值最大的buffer_size个路径
        mPaths.clear();
        sort(new_paths.begin(),new_paths.end(),greater<Path>());
        while(new_paths.size()>buffer_size)
        {
            new_paths.pop_back();
        }
        mPaths = new_paths;
    }
    auto opt_path = mPaths[0];
    opt_path.dumpResult(mDpmlConf->GetResultDIr());
    LOG_INFO << "Caculate done.Result:" << mDpmlConf->GetResultDIr().c_str();
}

void DpmlAlgrithm::InitAll()
{
    nanolog::initialize(nanolog::GuaranteedLogger(), "../log/", "nanolog", 1);
    LOG_INFO << "DPML Config File:" << mConfigFile.c_str();
    InitDpmlConf(mConfigFile);
    InitRadarData();
    Path::SetGroundTruth(mData->labels());
    InitPath();
    LOG_INFO << "InitAll Done.";
}

void DpmlAlgrithm::InitRadarData()
{
    mData = make_shared<RadarData>(mDpmlConf);
}

void DpmlAlgrithm::InitDpmlConf(const string& conf_file)
{
    mDpmlConf = make_shared<DpmlConf>(conf_file);
    n_radar = mDpmlConf->GetRadarNum();
    buffer_size = mDpmlConf->GetBufferSize();
    Path::SetRadarParams(mDpmlConf->GetRadarParams());
    Path::SetDealMiss(mDpmlConf->IsDealMiss());
    Path::SetBeta(mDpmlConf->GetBeta());
    LOG_INFO << "InitDpmlConf Done.";
    LOG_INFO << "DpmlConf:" << mDpmlConf->GetAllConf().c_str();
}

void DpmlAlgrithm::InitPath()
{
    for(int i = 0;i<n_radar;++i){
        // auto path = make_shared<Path>(n_radar,i,0,mData->toa()[0]);
        mPaths.emplace_back(n_radar,i,mData->toa(0),0);
    }
    if(mDpmlConf->IsDealSpurious())
    {
        mPaths.emplace_back(n_radar,SPURIOUS,mData->toa(0),0);
    }
    LOG_INFO << "InitPath Done. Paths size: " << mPaths.size();
    return;
}

