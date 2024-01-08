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
    inline string GetAllConf(){return json::dump(mDpmlConf);}
    inline int GetRadarNum(){return mDpmlConf["n_radar"].get<int>();}
    inline int GetBufferSize(){return mDpmlConf["buffer_size"].get<int>();}
    inline double GetBeta(){return mDpmlConf["beta"].get<double>();}
    inline bool IsDealMiss(){return mDpmlConf["deal_miss"].get<bool>();}
    inline bool IsDealSpurious(){return mDpmlConf["deal_spurious"].get<bool>();}
    inline const string GetDataFile() const {return mDpmlConf["data_file"].get<string>();}
    inline const string GetLabelFile() const {return mDpmlConf["label_file"].get<string>();}
    inline const string GetResultDIr() const {return mDpmlConf["result_dir"].get<string>();}
    json::value GetRadarParams(){return mDpmlConf["params"];}
private:
    json::value mDpmlConf;
};
using DpmlConfPtr = shared_ptr<DpmlConf>;



#endif