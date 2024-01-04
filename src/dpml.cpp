#include "dpml.h"
#include "conf.h"
#include "nanolog.hpp"


DpmlAlgrithm::DpmlAlgrithm()
{

}

void DpmlAlgrithm::start()
{

}

void DpmlAlgrithm::InitAll()
{
    nanolog::initialize(nanolog::GuaranteedLogger(), "../log/", "nanolog", 1);
    InitDpmlConf("../config/conf.json");
    InitRadarData();
    InitPath();
}



void update_likelihood(int k,double pt,Path& P)
{

}



//计算真实路径的似然值
double true_path_ll()
{
    return 0;
}
