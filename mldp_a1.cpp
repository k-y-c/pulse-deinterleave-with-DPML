#include<bits/stdc++.h>
using namespace std;

const int K = 2; // 辐射源数目
vector<double> toa; // toa数据
const string DATA_PATH = "./";
const string DATA_FILE_NAME = "toa.txt";
vector<pair<vector<int>,pair<vector<int>,double>>> paths;   //所有路径

vector<double> param_gaussian = {45000,100};  //高斯分布参数
#define GAUSSIAN 1
#define _USE_MATH_DEFINES  //pi
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
        vector<int>p = {i};
        vector<int>d(K,-1);
        d[i] = 1;
        paths.emplace_back(d,make_pair(p,1));
    }
    return;
}

int main(){
    read_data();
    init_paths();

    for(int i = 1;i<toa.size();++i){
        vector<pair<vector<int>,pair<vector<int>,double>>> new_paths;
        for(auto P:paths){
            auto dist = P.first; //最后脉冲数
            auto path = P.second.first; //路径
            auto ll = P.second.second; // 累计似然函数
            for(int k = 0;k<K;++k){
                auto __dist = dist;
                auto __path = path;
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
                
                // 3.将更新的路径加入新路径
                new_paths.emplace_back(__dist,make_pair(__path,__ll));
            }
        }

        // 4.将最后脉冲数相同的路径合并，保留最大似然函数的路径
        map<vector<int>,pair<vector<int>,double>> group;
        for(auto P:new_paths){
            auto dist = P.first; //最后脉冲数
            auto path = P.second.first; //路径
            auto ll = P.second.second; // 累计似然函数
            if(group.count(dist)){
                if(group[dist].second>0){
                    cout << "err";
                }
                else if(group[dist].second<ll){
                    group[dist] = P.second;
                }
            }
            else{
                group[dist] = P.second;
            }
        }

        // 5.将path替换
        paths.assign(group.begin(),group.end());
    }

    // 6. 取出似然函数最大的路径
    auto results = paths[0];
    for(auto P:paths){
        auto dist = P.first; //最后脉冲数
        auto path = P.second.first; //路径
        auto ll = P.second.second; // 累计似然函数
        if(ll > results.second.second){
            results = P;
        }
    }
    return 0;
}