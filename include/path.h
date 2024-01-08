#ifndef PATH
#define PATH

#include <vector>
#include <memory>
#include <map>
#include "configor/json.hpp"

#define SPURIOUS -1

using namespace std;
using namespace configor;

class Path{
    public:
        // K:辐射源个数，label:初始脉冲标签，ll：似然值(初始值为1)
        Path(int K,int label,double toa = -1,double ll = 1);
        double& likelihood(){return m_likelihood;}
        const double const_likelihood() const{return m_likelihood;} 
        void update(int pulse_idx,int label,double toa);
        void dumpResult(string file);
    public:
        static void SetGroundTruth(const vector<int>& labels);    
        static void SetRadarParams(const json::value& params);
        static void SetDealMiss(const bool deal_miss);
        static void SetBeta(const double beta);
        bool operator < (const Path& a) const{return m_likelihood < a.m_likelihood;}
        bool operator > (const Path& a) const{return m_likelihood > a.m_likelihood;}
    private:
        int LastPulseIdx(int k){return m_last_pulse_idx[k];}
        bool IsFirstPulse(int k)
        {
            return m_last_pulse_idx[k] == -1;
        }
        void AddPath(int k)
        {
            m_path.push_back(k);
        }

        void updateLikelihood(int label,double pt);
    
        void setFirstPulse(int pulse_idx,int label,int toa);
        json::value metrics();
    private:
        inline static json::value s_params;
        inline static bool m_deal_miss = false;
        inline static double m_beta;
        inline static vector<int> m_true_path;// ground truth
    private:
        vector<int> m_path; //路径
        map<int,int> m_last_pulse_idx; //上一个脉冲的位置
        map<int,double> m_last_pulse_time; //上一个脉冲的到达时间
        double m_likelihood; // 似然函数值   

        map<int,int> m_t_index; // for sinu

        // for sliding 
        map<int,double> m_last_pri;// 上一次的pri
        map<int,int> m_sliding_idx; // 滑步周期
        
        // for stagger
        map<int,vector<double>> m_alpha; 

        int m_spurious_num;
        
};
using PathPtr = shared_ptr<Path>;


#endif