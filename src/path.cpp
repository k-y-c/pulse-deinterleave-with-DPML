#include "path.h"
#include "util.h"

Path::Path(int K,int label,double ll,double pri0):
    m_likelihood(ll),
    m_path(1,label),
    m_last_pulse_idx(K,-1),
    m_last_pri(K,-1),
    m_sliding_period(K,1),
    m_alpha(K,vector<double>()),
    m_t_index(K,1)
{
    // // for sliding
    // m_last_pri[label] = pri0;
    // m_last_pulse_idx[label] = 0;

    // // for stagger
    // for(int i = 0;i<K;++i){
    //     m_alpha[i] = vector<double>(param_stagger[i].size(),-1);
    // }
    // for(int i = 0;i<param_stagger[label].size();++i){
    //     m_alpha[label][i] = 1.0/m_alpha[label].size()*gaussian_distribution(param_stagger[label][i],stagger_sigma,pri0);
    // }

    // // for sinu
    // // m_likelihood = log(gaussian_distribution(param_sinu[label][0]*sin(2*M_PI*param_sinu[label][1])+param_sinu[label][2],param_sinu[label][3],pri0));
    // m_t_index[label]++;
}