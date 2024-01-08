#include <bits/stdc++.h>
#include "nanolog.hpp"

#define GAUSSIAN1 1
#define GAUSSIAN2 2

#define _USE_MATH_DEFINES  //pi
using namespace std;

const int K = 2; // 辐射源数目
const double T_K = 1000000; //超时时间（单位：ns）
vector<double> toa; // toa数据
vector<int> label; // 数据标签
double beta_ = -10; // 似然因子
const int buffer_size = K*K*K*10; //路径buffer大小
// const string TYPE = "miss_"; //数据类型
const string TYPE = "spurious_"; //数据类型
const string RATE = "0.6"; // 率
const string DATA_PATH = "/home/cky/pulse-deinterleave-with-DPML/data/"+TYPE+RATE+"/";
const string DATA_FILE_NAME = "toa.txt";
const string LABEL_FILE_NAME = "label.txt";

class Path{
    public:
        // K:辐射源个数，label:初始脉冲标签，ll：似然值(初始值为1)
        Path(int K,int label,double ll = 1):m_likelihood(ll),
                    m_last_pulse_dist(K+1,-1),
                    m_path(1,label),
                    m_time(K+1,0){
                        m_last_pulse_dist[label] = 1;
                    }
        Path(vector<int>&last_pulse_dist,vector<int>&path,vector<double>&time,double ll):
        m_last_pulse_dist(last_pulse_dist),m_path(path),m_time(time),m_likelihood(ll)
        {};
        Path() = default;
        vector<int>& get_last_pulse_dist(){return m_last_pulse_dist;}
        vector<int>& get_path(){return m_path;}
        vector<double>& get_time(){return m_time;}
        double& get_likelihood(){return m_likelihood;}
    private:
        vector<int> m_last_pulse_dist; //最后脉冲数
        vector<int> m_path; //路径
        vector<double> m_time; //分配时长
        double m_likelihood; // 似然函数值       
};

vector<Path> paths;//所有路径

vector<double> param_gaussian1 = {45000,100};  //辐射源1的高斯分布参数
vector<double> param_gaussian2 = {60000,100};  //辐射源2的高斯分布参数

double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}

double likelihood(int type,double value){
    double res = 0;
    switch (type)
    {
    case GAUSSIAN1:
        return log(gaussian_distribution(param_gaussian1[0],param_gaussian1[1],value));
        break;
    case GAUSSIAN2:
        return log(gaussian_distribution(param_gaussian2[0],param_gaussian2[1],value));
        break;
    default:
        break;
    }
    return 0;
}

//读取toa、label数据
void read_data(){
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
        label.push_back(val == -1?K:val);
    }
    //剔除漏脉冲数据
}

//初始化路径
void init_paths(){
    for(int i = 0;i<K;++i){
        paths.emplace_back(forward<Path>(Path(K,i,0)));
    }
    paths.emplace_back(forward<Path>(Path(K,K,beta_)));
    return;
}

//计算真实路径的似然值
double true_path_ll(){
    vector<int> idx;
    for(int i = 0;i<toa.size();++i){
        if(label[i]==0){
            idx.push_back(i);
        }
    }
    double ll = 0;
    for(int i = 0;i<idx.size()-1;++i){
        int l = toa[idx[i]];
        int r = toa[idx[i+1]];
        ll += likelihood(GAUSSIAN1,r-l);
    }
    idx.clear();
    for(int i = 0;i<toa.size();++i){
        if(label[i]==1){
            idx.push_back(i);
        }
    }
    for(int i = 0;i<idx.size()-1;++i){
        int l = toa[idx[i]];
        int r = toa[idx[i+1]];
        ll += likelihood(GAUSSIAN2,r-l);
    }
    return ll;
}

void parse_args(int argc,char* argv[]){
    if(argc>1){
        beta_ = atoi(argv[1]);
    }
}

int main(int argc,char * argv[]){
    parse_args(argc,argv);
    nanolog::initialize(nanolog::GuaranteedLogger(), "/home/cky/pulse-deinterleave-with-DPML/log/", "nanolog", 1);
    read_data();
    init_paths();

    for(int i = 1;i<toa.size();++i){
        vector<Path> new_paths;
        for(auto P:paths){
            auto& dist = P.get_last_pulse_dist(); //最后脉冲数
            auto& path = P.get_path(); //路径
            auto& rtime = P.get_time(); //分配时长
            auto& ll = P.get_likelihood(); // 累计似然函数
            
            for(int k = 0;k<=K;++k){
                auto __dist = dist;
                auto __path = path;
                auto __rtime = rtime;
                auto __ll = ll;
                __path.push_back(k); // 更新路径
                // 1.如果该路径曾经分配过脉冲给源k，即可以计算似然函数
                // 计算杂散脉冲的似然值
                if(k == K){
                    __ll += beta_;
                }
                else if(__dist[k]!=-1){
                    double l = toa[i-__dist[k]];
                    double r = toa[i];
                    if(__ll > 0){
                        __ll = likelihood(k==0?GAUSSIAN1:GAUSSIAN2,r-l);
                        // __ll = max(likelihood(k==0?GAUSSIAN1:GAUSSIAN2,r-l),likelihood(k==0?GAUSSIAN1:GAUSSIAN2,(r-l)/2)); 
                    }
                    else{
                        __ll += likelihood(k==0?GAUSSIAN1:GAUSSIAN2,r-l);
                        // __ll += max(likelihood(k==0?GAUSSIAN1:GAUSSIAN2,r-l),likelihood(k==0?GAUSSIAN1:GAUSSIAN2,(r-l)/2));
                    }
                }

                // 2.更新最后脉冲数
                for(int j = 0;j<K+1;++j){
                    __dist[j] = j==k?1:__dist[j]==-1?-1:__dist[j]+1;
                }

                // 2.1 更新最后时间,略掉超时路径
                double t = -1;
                for(int j = 0;j<K;++j){
                    __rtime[j] = j==k?0:(__rtime[j]+toa[i]-toa[i-1]);
                    t = max(t,__rtime[j]);
                }
                if(t>T_K){
                    continue;
                }
                // 3.将更新的路径加入新路径
                new_paths.emplace_back(forward<Path>(Path(__dist,__path,__rtime,__ll)));
            }
        }

        // 4.将最后脉冲数相同的路径合并，保留最大似然函数的路径
        map<vector<int>,Path> group;
        for(auto P:new_paths){
            auto dist = P.get_last_pulse_dist(); //最后脉冲数
            auto ll = P.get_likelihood(); // 累计似然函数
            if(group.count(dist)){
                if(group[dist].get_likelihood()>0){
                    LOG_CRIT << "err";
                }
                else if(group[dist].get_likelihood()<ll){
                    group[dist] = P;
                }
            }
            else{
                group[dist] = P;
            }
        }

        // 5.将path替换
        paths.clear();
        auto cmp = [](Path l,Path r){return l.get_likelihood()>r.get_likelihood();};
        for(auto& p:group){
            paths.push_back(p.second);
        }
        sort(paths.begin(),paths.end(),cmp);
        while(paths.size()>buffer_size){paths.pop_back();}

        // vector<pair<double,Path>> buffer;
        // for(auto& [_,p]:group){
        //     buffer.emplace(p.get_likelihood(),p);
        //     if(buffer.size()>buffer_size){
        //         buffer.erase(buffer.begin());
        //     }
        // }
        // for(auto& [_,p]:buffer){
        //     paths.push_back(p);
        // }


        // paths.assign(group.begin(),group.end());
    }

    // 6. 取出似然函数最大的路径
    auto results = paths[0];
    for(auto P:paths){
        auto ll = P.get_likelihood(); // 累计似然函数
        if(ll > results.get_likelihood()){
            results = P;
        }
    }
    int cnt_wrong = 0;
    auto path = results.get_path();
    for(int i = 0;i<path.size();++i){
        if(path[i]!=label[i]){
            cnt_wrong += 1;
            // LOG_INFO << "wrong pulse idx: " << i << "; True: " << label[i] << "; Recognized: " << path[i];
        }
    }
    LOG_INFO << "algorithm: mldp_a2_for_spurious";
    LOG_INFO << TYPE.c_str() << RATE.c_str() << "  beta:" << beta_;
    LOG_INFO << "Wrong deinterleaved pulses rate: "<< 1.0*cnt_wrong/path.size() ;
    // LOG_INFO << beta_ << " " << 1.0*cnt_wrong/path.size();
    return 0;
}