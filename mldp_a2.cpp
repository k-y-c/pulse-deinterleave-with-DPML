#include<bits/stdc++.h>
#define GAUSSIAN 1
#define _USE_MATH_DEFINES  //pi
using namespace std;

const int K = 2; // 辐射源数目
const double T_K = 100000; //超时时间（单位：ns）
vector<double> toa; // toa数据
const string DATA_PATH = "./";
const string DATA_FILE_NAME = "toa.txt";

class Path{
    public:
        // K:辐射源个数，label:初始脉冲标签，ll：似然值
        Path(int K,int label,double ll = 1):m_likelihood(ll),
                    m_last_pulse_dist(K,-1),
                    m_path(1,label),
                    m_time(K,0){
                        m_last_pulse_dist[label] = 1;
                    }
        
        //
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

vector<double> param_gaussian = {45000,100};  //高斯分布参数

double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}

double likelihood(int type,double value){
    double res = 0;
    switch (type)
    {
    case GAUSSIAN:
        return log(gaussian_distribution(param_gaussian[0],param_gaussian[1],value));
        break;
    
    default:
        break;
    }
    return 0;
}

void read_data(){
    fstream fs;
    fs.open(DATA_PATH+DATA_FILE_NAME);
    string s;
    while(fs >> s){
        double val = stod(s);
        toa.push_back(val*1e9);
    }
}

void init_paths(){
    for(int i = 0;i<K;++i){
        paths.emplace_back(forward<Path>(Path(K,i,1.0)));
    }
    return;
}

int main(){
    read_data();
    init_paths();

    for(int i = 1;i<toa.size();++i){
        vector<Path> new_paths;
        for(auto P:paths){
            auto& dist = P.get_last_pulse_dist(); //最后脉冲数
            auto& path = P.get_path(); //路径
            auto& rtime = P.get_time(); //分配时长
            auto& ll = P.get_likelihood(); // 累计似然函数
            
            for(int k = 0;k<K;++k){
                auto __dist = dist;
                auto __path = path;
                auto __rtime = rtime;
                auto __ll = ll;
                __path.push_back(k); // 更新路径
                // 1.如果该路径曾经分配过脉冲给源k，即可以计算似然函数
                if(__dist[k]!=-1){
                    double l = toa[i-__dist[k]];
                    double r = toa[i];
                    if(__ll > 0){
                        __ll = likelihood(GAUSSIAN,r-l);
                    }
                    else{
                        __ll += likelihood(GAUSSIAN,r-l);
                    }
                }

                // 2.更新最后脉冲数
                for(int j = 0;j<K;++j){
                    __dist[j] = j==k?1:__dist[j]==-1?-1:__dist[j]+1;
                }

                // 2.1 更新最后时间,略掉超时路径
                double t = -1;
                for(int j = 0;j<K;++j){
                    __rtime[j] = j==k?0:__rtime[j]+toa[i]-toa[i-1];
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
                    cout << "err";
                }
                else if(group[dist].get_likelihood()<ll){
                    group[dist].get_likelihood() = ll;
                }
            }
            else{
                group[dist] = P;
            }
        }

        // 5.将path替换
        paths.clear();
        for(auto& p:group){
            paths.push_back(p.second);
        }
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
    return 0;
}