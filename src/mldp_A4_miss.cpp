#include <bits/stdc++.h>
#include "nanolog.hpp"

#define GAUSSIAN1 1
#define GAUSSIAN2 2

#define _USE_MATH_DEFINES  //pi
using namespace std;

const int K = 3; // 辐射源数目
const double T_K = 1000; //超时时间（单位：ns）
vector<double> toa; // toa数据
vector<int> label; // 数据标签
const double beta_ = -10; // 似然因子
const int buffer_size = K*K*K*K; //路径buffer大小
const bool DEAL_MISS = false; // 修正漏脉似然因子
const string TYPE = "miss/"; //数据类型
// const string TYPE = "spurious_";
 string MODULATION = "jitter"; //调制类型
 string RATE = "45"; // 率

const string DATA_FILE_NAME = "toa.txt";
const string LABEL_FILE_NAME = "label.txt";




vector<vector<double>> param_jitter = {{31,1},{55,1},{67,1}};  //3个辐射源的jitter分布参数

vector<vector<double>> param_sliding = {{16,1,6},{22,1,4},{9,1,8}};  //3个辐射源的sliding分布参数

vector<vector<double>> param_stagger = {{43,34,67,56},{47,61,55},{23,76,59}};  // stagger分布参数
double stagger_sigma = 0.5;  // stagger分布参数

vector<vector<double>> param_sinu = {{3,0.02,40,0.5},{4,0.015,35,0.5},{5,0.01,30,0.5}};  // sinu分布参数

double gaussian_distribution(double mu,double sigma,double value);

class Path{
    public:
        // K:辐射源个数，label:初始脉冲标签，ll：似然值(初始值为1)
        Path(int K,int label,double ll = 1,double pri0 = -1):m_likelihood(ll),
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
        Path() = default;
        vector<int>& last_pulse_idx(){return m_last_pulse_idx;}
        vector<int>& path(){return m_path;}
        vector<double>& last_pri(){return m_last_pri;}
        double& likelihood(){return m_likelihood;}
        const double const_likelihood() const{return m_likelihood;} 
        vector<int>& sliding_period(){return m_sliding_period;}
        vector<vector<double>>& alpha(){return m_alpha;}
        vector<int>& t_idx(){return m_t_index;}
    private:
        vector<int> m_path; //路径
        vector<int> m_last_pulse_idx; //上一个脉冲的位置
        double m_likelihood; // 似然函数值   

        // for sinu
        vector<int> m_t_index; 

        // for stagger
        vector<vector<double>> m_alpha;

        // for sliding 
        vector<double> m_last_pri;// 上一次的pri
        vector<int> m_sliding_period; // 滑步周期
};

vector<Path> paths;//所有路径



double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}


void update_likelihood(int k,double pt,Path& P){

    if(MODULATION == "jitter"){
        // jitter
        double ll = log(gaussian_distribution(param_jitter[k][0],param_jitter[k][1],pt));
        double ll_2 = log(gaussian_distribution(param_jitter[k][0],param_jitter[k][1],pt/2));
        double ll_3 = log(gaussian_distribution(param_jitter[k][0],param_jitter[k][1],pt/3));
        P.likelihood() += max(ll_3,max(ll,ll_2));
        return;
    }
    else if(MODULATION == "sinusoidal"){
        // sinu
        double res = 0;
        double ll = log(gaussian_distribution(param_sinu[k][0]*sin(2*M_PI*param_sinu[k][1]*P.t_idx()[k])+param_sinu[k][2],param_sinu[k][3],pt));
        double ll_2 = log(gaussian_distribution(param_sinu[k][0]*sin(2*M_PI*param_sinu[k][1]*P.t_idx()[k])+param_sinu[k][2],param_sinu[k][3],pt/2));
        double ll_3 = log(gaussian_distribution(param_sinu[k][0]*sin(2*M_PI*param_sinu[k][1]*P.t_idx()[k])+param_sinu[k][2],param_sinu[k][3],pt/3));
        P.likelihood() += max(ll_3,max(ll,ll_2));
        ++P.t_idx()[k];
        if(ll_2 == max(ll_3,max(ll,ll_2)))++P.t_idx()[k];
        if(ll_3 == max(ll_3,max(ll,ll_2)))++++P.t_idx()[k];
        return;
    }
    else if(MODULATION == "stagger"){
        //stagger
        
        if(P.alpha()[k][0]<0){
            for(int i = 0;i<param_stagger[k].size();++i){
                double ll = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt);
                double ll_2 = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt/2);
                double ll_3 = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt/3);
                double fai = max(ll_3,max(ll,ll_2));
                P.alpha()[k][i] = 1.0/P.alpha()[k].size()*fai;
            }
        }
        else{
            double alpha_t = accumulate(P.alpha()[k].begin(),P.alpha()[k].end(),0.0);
            for(int i = 0;i<param_stagger[k].size();++i){
                double ll = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt);
                double ll_2 = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt/2);
                double ll_3 = gaussian_distribution(param_stagger[k][i],stagger_sigma,pt/3);
                double fai = max(ll_3,max(ll,ll_2));
                P.alpha()[k][i] = 1.0/P.alpha()[k].size()*alpha_t*fai;
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
        // if(P.sliding_period()[k]%(int)param_sliding[k][2] != 0){
        double ll = log(gaussian_distribution(param_sliding[k][0]+P.last_pri()[k],param_sliding[k][1],pt));
        double ll_2 = log(gaussian_distribution(param_sliding[k][0]+P.last_pri()[k],param_sliding[k][1],pt/2-param_sliding[k][0]/2));
        double ll_3 = log(gaussian_distribution(param_sliding[k][0]+P.last_pri()[k],param_sliding[k][1],pt/3-param_sliding[k][0]));
        // }
        res = max(ll_3,max(ll,ll_2));
        // res = max(ll,ll_2);
            // P.last_pri()[k] = ll>=ll_2?pt:ll_2>=ll_3?pt/2+param_sliding[k][0]/2:pt/3+param_sliding[k][0];
            // if(ll_2>ll)
            //     ++P.sliding_period()[k];
        P.last_pri()[k] = pt;
        if(ll_3 > ll_2 && ll_3 > ll)P.last_pri()[k] = pt/3+param_sliding[k][0];
        if(ll_2 > ll_3 && ll_2 > ll)P.last_pri()[k] = pt/2+param_sliding[k][0]/2;
        // if(ll_2 == res)P.last_pri()[k] = pt/2;

        // res = ll;
        // P.last_pri()[k] = ll>=ll_2?pt:pt/2+param_sliding[k][0]/2;

        P.likelihood() += res;
        return ;
    }

}

//读取toa、label数据
void read_data(){
    const string DATA_PATH = "/home/cky/pulse-deinterleave-with-DPML/data/"+TYPE+MODULATION+"/"+RATE+"/";
    fstream fs;
    fs.open(DATA_PATH+DATA_FILE_NAME);
    string s;
    while(fs >> s){
        double val = stod(s);
        toa.push_back(val);
    }
    fs.close();
    fs.open(DATA_PATH+LABEL_FILE_NAME);
    while(fs >> s){
        int val = stoi(s);
        label.push_back(val);
    }
    //剔除漏脉冲数据
}

//初始化路径
void init_paths(){
    for(int i = 0;i<K;++i){
        paths.emplace_back(forward<Path>(Path(K,i,0,toa[0])));
    }
    return;
}

//计算真实路径的似然值
double true_path_ll(){
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


void parse_args(int argc,char* argv[]){
    if(argc>1){
        // beta_ = atoi(argv[1]);
        MODULATION = string(argv[1]);
        RATE = string(argv[2]);
    }
}

int main(int argc,char* argv[]){
    parse_args(argc,argv);
    nanolog::initialize(nanolog::GuaranteedLogger(), "/home/cky/pulse-deinterleave-with-DPML/log/", "nanolog", 1);
    read_data();
    init_paths();

    for(int i = 1;i<toa.size();++i){
        vector<Path> new_paths;
        for(auto P:paths){
            for(int k = 0;k<K;++k){
                auto __P = P;
                __P.path().push_back(k); // 更新路径
                // 1.如果该路径曾经分配过脉冲给源k，即可以计算似然函数
                // 计算杂散脉冲的似然值
                if(__P.last_pulse_idx()[k]!=-1){
                    double pt = toa[i]-toa[__P.last_pulse_idx()[k]];
                    update_likelihood(k,pt,__P);
                }
                else{
                    update_likelihood(k,toa[i],__P);
                }

                // 2.更新最后脉冲位置
                __P.last_pulse_idx()[k] = i;

                // 3.将更新的路径加入新路径
                new_paths.emplace_back(__P);
            }
        }

        // 4.保留似然值最大的buffer_size个路径
        paths.clear();
        auto cmp = [](const Path& l,const Path& r){return l.const_likelihood()>r.const_likelihood();};
        sort(new_paths.begin(),new_paths.end(),cmp);
        while(new_paths.size()>buffer_size){new_paths.pop_back();}
        paths = new_paths;
    }

    // 6. 取出似然函数最大的路径
    auto results = paths[0];
    for(auto P:paths){
        auto ll = P.likelihood(); // 累计似然函数
        if(ll > results.likelihood()){
            results = P;
        }
    }
    int cnt_wrong = 0;
    auto path = results.path();
    for(int i = 0;i<path.size();++i){
        if(path[i]!=label[i]){
            cnt_wrong += 1;
            // LOG_INFO << "wrong pulse idx: " << i << "; True: " << label[i] << "; Recognized: " << path[i];
        }
    }
    // LOG_INFO << "improved: ";
    // for(int i = 0;i<100;++i){
    //     LOG_INFO << path[i];
    // }
    // LOG_INFO << "Total pulses: " << path.size();
    // LOG_INFO << "algorithm: improved_mldp_a2";
    // LOG_INFO << TYPE.c_str() << RATE.c_str();
    // LOG_INFO << "acc: "<< 1 - 1.0*cnt_wrong/path.size() ;
    LOG_INFO << "improved: " << MODULATION.c_str()<<"/" <<TYPE.c_str() << RATE.c_str() <<" acc: "<< 1 - 1.0*cnt_wrong/path.size();
    // LOG_INFO << "Likelihood: " << results.likelihood();
    // LOG_INFO << "True path Likelihood: " << true_path_ll();
    return 0;
}