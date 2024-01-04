#include <bits/stdc++.h>
#include "nanolog.hpp"
#include "global.h"
#include "dpml.h"


int main(int argc,char* argv[]){
    init(argc,argv);

    for(int i = 1;i<toa.size();++i){
        vector<Path> new_paths;
        for(auto P:paths){
            for(int k = 0;k<K;++k){
                P.path().push_back(k); // 更新路径
                // 1.如果该路径曾经分配过脉冲给源k，即可以计算似然函数
                // 计算杂散脉冲的似然值
                if(P.last_pulse_idx()[k]!=-1){
                    double pt = toa[i]-toa[P.last_pulse_idx()[k]];
                    update_likelihood(k,pt,P);
                }
                else{
                    update_likelihood(k,toa[i],P);
                }

                // 2.更新最后脉冲位置
                P.last_pulse_idx()[k] = i;

                // 3.将更新的路径加入新路径
                new_paths.emplace_back(P);
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
    // LOG_INFO << "original: ";
    // for(int i = 0;i<100;++i){
    //     LOG_INFO << path[i];
    // }
    // LOG_INFO << "Total pulses: " << path.size();
    // LOG_INFO << "algorithm: improved_mldp_a2";
    // LOG_INFO << TYPE.c_str() << RATE.c_str();
    // LOG_INFO << "acc: "<< 1 - 1.0*cnt_wrong/path.size() ;
    LOG_INFO << "original: " << MODULATION.c_str()<<"/" <<TYPE.c_str() << RATE.c_str() <<" acc: "<< 1 - 1.0*cnt_wrong/path.size();
    // LOG_INFO << "Likelihood: " << results.likelihood();
    // LOG_INFO << "True path Likelihood: " << true_path_ll();
    return 0;
}