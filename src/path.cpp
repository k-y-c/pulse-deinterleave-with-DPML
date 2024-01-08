#include "path.h"
#include "util.h"
#include <numeric>
#include <fstream>
#include <iostream>
using namespace std;

Path::Path(int K,int label,double toa,double ll):
    m_likelihood(ll),
    m_path(1,label),
    m_spurious_num(0)
{
    // 初始化脉冲位置、时间
    for(int i = 0;i<K;++i)
    {
        m_last_pulse_idx[i] = -1;
        m_last_pulse_time[i] = -1;
    }
    setFirstPulse(0,label,toa);
}

void Path::SetRadarParams(const json::value& params)
{
    s_params = params;
}

void Path::SetGroundTruth(const vector<int>& labels)
{
    m_true_path = labels;
}

void Path::SetDealMiss(const bool deal_miss)
{
    m_deal_miss = deal_miss;
}

void Path::SetBeta(const double beta)
{
    m_beta = beta;
}

void Path::setFirstPulse(int pulse_idx,int label,int toa)
{
    if(label == SPURIOUS)
    {
        ++m_spurious_num;
        m_likelihood += m_beta;
        return;
    }
    m_last_pulse_idx[label] = pulse_idx;
    m_last_pulse_time[label] = toa;
    string modulation = s_params[label]["modulation"].get<string>();
    if(modulation == "sinusoidal")
    {
        m_t_index[label] = 0;
    }
    else if(modulation == "sliding")
    {
        m_last_pri[label] = -1;
        m_sliding_idx[label] = 1;
    }
    else if(modulation == "stagger")
    {
        m_alpha[label] = vector<double>(s_params[label]["num"].get<int>(),-1);
    } 
}

void Path::update(int pulse_idx,int label,double toa)
{
    AddPath(label);
    if(IsFirstPulse(label))
    {
        setFirstPulse(pulse_idx,label,toa);
    }
    else
    {
        double pt = toa - m_last_pulse_time[label];
        updateLikelihood(label,pt);
    }
    m_last_pulse_idx[label] = pulse_idx;
    m_last_pulse_time[label] = toa;
}

void Path::updateLikelihood(int label,double pt)
{
    if(label == SPURIOUS)
    {
        m_likelihood += m_beta;
        return;
    }
    string modulation = s_params[label]["modulation"].get<string>();
    if(modulation == "jitter"){
        double mu = s_params[label]["mu"].get<double>();
        double sigma = s_params[label]["sigma"].get<double>();
        if(m_deal_miss)
        {
            double ll = log(gaussian_distribution(mu,sigma,pt));
            double ll_2 = log(gaussian_distribution(mu,sigma,pt/2));
            double ll_3 = log(gaussian_distribution(mu,sigma,pt/3));
            m_likelihood += max(ll_3,max(ll,ll_2));
        }
        else
        {
            double ll = log(gaussian_distribution(mu,sigma,pt));
            m_likelihood += ll;
        }
        return;
    }
    else if(modulation == "sinusoidal"){
        double amplitude = s_params[label]["amplitude"].get<double>();
        double frequency = s_params[label]["frequency"].get<double>();
        double bias = s_params[label]["bias"].get<double>();
        double sigma = s_params[label]["sigma"].get<double>();
        double fai = s_params[label]["fai"].get<double>();
        if(m_deal_miss)
        {
            double ll = log(gaussian_distribution(amplitude*sin(2*M_PI*frequency*m_t_index[label])+bias,sigma,pt));
            double ll_2 = log(gaussian_distribution(amplitude*sin(2*M_PI*frequency*m_t_index[label])+bias,sigma,pt/2));
            double ll_3 = log(gaussian_distribution(amplitude*sin(2*M_PI*frequency*m_t_index[label])+bias,sigma,pt/3));
            m_likelihood += max(ll_3,max(ll,ll_2));
            m_t_index[label] += 1;
            if(ll_2 == max(ll_3,max(ll,ll_2)))m_t_index[label] += 1;
            if(ll_3 == max(ll_3,max(ll,ll_2)))m_t_index[label] += 2;
        }
        else
        {
            double ll = log(gaussian_distribution(amplitude*sin(2*M_PI*frequency*m_t_index[label])+bias,sigma,pt));
            m_likelihood += ll;
            m_t_index[label] += 1;
        }
        return;
    }
    else if(modulation == "stagger"){
        double ksai = 0;
        if(m_alpha[label][0]<0){
            for(int i = 0;i<m_alpha[label].size();++i){
                double fai = 0;
                if(m_deal_miss)
                {
                    double ll = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt);
                    double ll_2 = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt/2);
                    double ll_3 = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt/3);
                    fai = max(ll_3,max(ll,ll_2));
                }
                else
                {
                    double ll = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt);
                    fai = ll;
                }
                m_alpha[label][i] = 1.0/m_alpha[label].size()*fai;
            }
        }
        else{
            ksai = accumulate(m_alpha[label].begin(),m_alpha[label].end(),0.0);
            for(int i = 0;i<m_alpha[label].size();++i){
                double fai = 0;
                if(m_deal_miss)
                {
                    double ll = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt);
                    double ll_2 = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt/2);
                    double ll_3 = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt/3);
                    fai = max(ll_3,max(ll,ll_2));
                }
                else
                {
                    double ll = gaussian_distribution(s_params[label]["mu"][i].get<double>(),s_params[label]["sigma"][i].get<double>(),pt);
                    fai = ll;
                }
                m_alpha[label][i] = 1.0/m_alpha[label].size()*ksai*fai;
            }
        }
        double res = 0;
        res = log(accumulate(m_alpha[label].begin(),m_alpha[label].end(),0.0));
        if(ksai > 0)
        {
            m_likelihood = m_likelihood + res - log(ksai);
        }
        else
        {
            m_likelihood += res;
        }
        return;
    }
    else if(modulation == "sliding"){
        //sliding
        if(m_last_pri[label]<0){
            m_last_pri[label] = pt;
            return ;
        }
        double res = 0;
        if(m_sliding_idx[label]%s_params[label]["period"].get<int>() != 0)
        {
            if(m_deal_miss)
            {
                double ll = log(gaussian_distribution(s_params[label]["alpha"].get<double>()+m_last_pri[label],s_params[label]["sigma"].get<double>(),pt));
                double ll_2 = log(gaussian_distribution(s_params[label]["alpha"].get<double>()+m_last_pri[label],s_params[label]["sigma"].get<double>(),pt/2));
                double ll_3 = log(gaussian_distribution(s_params[label]["alpha"].get<double>()+m_last_pri[label],s_params[label]["sigma"].get<double>(),pt/3));
                res = max(ll_3,max(ll,ll_2));
                if(ll_3 > ll_2 && ll_3 > ll)
                {
                    m_sliding_idx[label] += 3;
                    m_last_pri[label] = pt/3+s_params[label]["alpha"].get<double>();
                }
                else if(ll_2 > ll_3 && ll_2 > ll)
                {
                    m_sliding_idx[label] += 2;
                    m_last_pri[label] = pt/2+s_params[label]["alpha"].get<double>()/2;
                }
                else
                {
                    m_sliding_idx[label] += 1;
                    m_last_pri[label] = pt;
                }
            }
            else
            {
                double ll = log(gaussian_distribution(s_params[label]["alpha"].get<double>()+m_last_pri[label],s_params[label]["sigma"].get<double>(),pt));
                res = ll;
                ++m_sliding_idx[label];
                m_last_pri[label] = pt;
            }
        }
        else{
            ++m_sliding_idx[label];
        }
        m_likelihood += res;
        return ;
    }
}

json::value Path::metrics()
{
    int cnt_wrong = 0;
    int cnt_all = m_path.size();
    for(int i = 0;i<m_path.size();++i){
        if(m_path[i]!=m_true_path[i]){
            cnt_wrong += 1;
        }
    }
    double err_rate = 1.0*cnt_wrong/cnt_all;
    double accuracy = 1 - err_rate;
    return json::object{
        {"error_rate",err_rate},
        {"accuracy",accuracy}
    };
}

void Path::dumpResult(string file)
{
    json::value j = json::object{
        {"likelihood",m_likelihood},
        {"metrics",metrics()},
        {"output_label",m_path},
        {"true_label",m_true_path},
    };
    std::ofstream ofs(file);
    ofs << std::setw(4) << json::wrap(j) << std::endl;
}