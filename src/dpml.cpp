#include "dpml.h"
#include "global.h"
#include "nanolog.hpp"

Path::Path(int K,int label,double ll,double pri0):
    m_likelihood(ll),
    m_path(1,label),
    m_last_pulse_idx(K,-1),
    m_last_pri(K,-1),
    m_sliding_period(K,1),
    m_alpha(K,vector<double>()),
    m_t_index(K,1)
{
    // for sliding
    m_last_pri[label] = pri0;
    m_last_pulse_idx[label] = 0;

    // for stagger
    for(int i = 0;i<K;++i){
        m_alpha[i] = vector<double>(param_stagger[i].size(),-1);
    }
    for(int i = 0;i<param_stagger[label].size();++i){
        m_alpha[label][i] = 1.0/m_alpha[label].size()*gaussian_distribution(param_stagger[label][i],stagger_sigma,pri0);
    }

    // for sinu
    // m_likelihood = log(gaussian_distribution(param_sinu[label][0]*sin(2*M_PI*param_sinu[label][1])+param_sinu[label][2],param_sinu[label][3],pri0));
    m_t_index[label]++;
}

double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}

void update_likelihood(int k,double pt,Path& P)
{

    if(MODULATION == "jitter"){
        // jitter
        P.likelihood() += log(gaussian_distribution(param_jitter[k][0],param_jitter[k][1],pt));
        return;
    }
    else if(MODULATION == "sinusoidal"){
        // sinu
        double res = 0;
        P.likelihood() += log(gaussian_distribution(param_sinu[k][0]*sin(2*M_PI*param_sinu[k][1]*P.t_idx()[k])+param_sinu[k][2],param_sinu[k][3],pt));
        ++P.t_idx()[k];
        return;
    }
    else if(MODULATION == "stagger"){
        //stagger
        if(P.alpha()[k][0]<0){
            for(int i = 0;i<param_stagger[k].size();++i){
                P.alpha()[k][i] = 1.0/P.alpha()[k].size()*gaussian_distribution(param_stagger[k][i],stagger_sigma,pt);
            }
        }
        else{
            double alpha_t = accumulate(P.alpha()[k].begin(),P.alpha()[k].end(),0.0);
            for(int i = 0;i<param_stagger[k].size();++i){
                P.alpha()[k][i] = 1.0/P.alpha()[k].size()*alpha_t*gaussian_distribution(param_stagger[k][i],stagger_sigma,pt);
            }
        }
        double res = 0;
        for(int i = 0;i<K;++i){
            if(P.alpha()[k][0]>0){
                res += log(accumulate(P.alpha()[k].begin(),P.alpha()[k].end(),0.0));
            }
        }
        P.likelihood() = res;
        return;
    }
    else if(MODULATION == "sliding"){
        //sliding
        if(P.last_pri()[k]<0){
            P.last_pri()[k] = pt;
            return ;
        }
        double res = 0;
        double ll = 0,ll_2 = 0;
        // if(P.sliding_period()[k]%(int)param_sliding[k][2] != 0){
            ll = log(gaussian_distribution(param_sliding[k][0]+P.last_pri()[k],param_sliding[k][1],pt));
            ll_2 = log(gaussian_distribution(param_sliding[k][0]+P.last_pri()[k],param_sliding[k][1],pt/2-param_sliding[k][0]/2));
        // }
        // ++P.sliding_period()[k];
        if(DEAL_MISS){
            res = max(ll,ll_2);
            // P.last_pri()[k] = ll>=ll_2?pt:pt/2;
            // if(ll_2>ll)
            //     ++P.sliding_period()[k];
        }
        else{
            res = ll;
            P.last_pri()[k] = pt;
        }
        P.likelihood() += res;
        return ;
    }

}

//初始化路径
void init_paths()
{
    for(int i = 0;i<K;++i){
        paths.emplace_back(forward<Path>(Path(K,i,0,toa[0])));
    }
    return;
}

//计算真实路径的似然值
double true_path_ll()
{
    Path _P(K,label[0],0,toa[0]);
    for(int i = 1;i<toa.size();++i){
        _P.path().push_back(label[i]);
        // 1.如果该路径曾经分配过脉冲给源k，即可以计算似然函数
        // 计算杂散脉冲的似然值
        if(_P.last_pulse_idx()[label[i]]!=-1){
            double pt = toa[i]-toa[_P.last_pulse_idx()[label[i]]];
            update_likelihood(label[i],pt,_P);
        }
        else{
            update_likelihood(label[i],toa[i],_P);
        }

        // 2.更新最后脉冲位置
        _P.last_pulse_idx()[label[i]] = i;
    }

    return _P.likelihood();
}

void init(int argc,char* argv[]){
    parse_args(argc,argv);
    nanolog::initialize(nanolog::GuaranteedLogger(), "/home/cky/pulse-deinterleave-with-DPML/log/", "nanolog", 1);
    read_data();
    init_paths();
}