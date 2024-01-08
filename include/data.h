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
    
    double toa(int i){return mToa[i];}
    int label(int i){return mLabel[i];}
    const vector<int>& labels() const{return mLabel;}
    int size(){return mDataSize;}
    void InitData();
    
private:
    DpmlConfPtr mDpmlConf;
    vector<double> mToa;
    vector<int> mLabel;
    int mDataSize;
};

using RadarDataPtr = shared_ptr<RadarData>;

#endif