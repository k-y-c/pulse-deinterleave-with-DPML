#ifndef DATA
#define DATA

#include <vector>
#include <memory>
#include "conf.h"
using namespace std;

class RadarData
{
public:
    RadarData(DpmlConfPtr conf);
    
    vector<double>& toa(){return mToa;}
    vector<int>& label(){return mLabel;}

    void InitData();
    
private:
    DpmlConfPtr mDpmlConf;
    vector<double> mToa;
    vector<int> mLabel;
};

using RadarDataPtr = shared_ptr<RadarData>;

#endif