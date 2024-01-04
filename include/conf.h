#ifndef CONF
#define CONF

#include <memory>
#include <string>
#include "configor/json.hpp"

using namespace std;
using namespace configor;

class DpmlConf
{
public:
    DpmlConf(string conf_file);
    inline int GetRadarNum(){return mDpmlConf["n_radar"].get<int>();}
    inline double GetTimeK(){return mDpmlConf["t_k"].get<double>();}
    inline double GetBeta(){return mDpmlConf["beta"].get<double>();}
    inline bool IsDealMiss(){return mDpmlConf["deal_miss"].get<bool>();}
    inline bool IsDealSpurious(){return mDpmlConf["deal_spurious"].get<bool>();}
    inline string GetDataFile(){return mDpmlConf["data_file"].get<string>();}
    inline string GetLabelFile(){return mDpmlConf["label_file"].get<string>();}
    json::value GetRadarParams(){return mDpmlConf["params"];}
private:
    json::value mDpmlConf;
};
using DpmlConfPtr = shared_ptr<DpmlConf>;



#endif