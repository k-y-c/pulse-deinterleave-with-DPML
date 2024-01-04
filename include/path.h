#include <vector>
#include <memory>
#include "configor/json.hpp"
using namespace std;
using namespace configor;

class Path{
    public:
        // K:辐射源个数，label:初始脉冲标签，ll：似然值(初始值为1)
        Path(int K,int label,double ll = 1,double pri0 = -1);
        Path() = default;
        vector<int>& last_pulse_idx(){return m_last_pulse_idx;}
        vector<int>& path(){return m_path;}
        vector<double>& last_pri(){return m_last_pri;}
        double& likelihood(){return m_likelihood;}
        const double const_likelihood() const{return m_likelihood;} 
        vector<int>& sliding_period(){return m_sliding_period;}
        vector<vector<double>>& alpha(){return m_alpha;}
        vector<int>& t_idx(){return m_t_index;}
    
    public:
        static void SetRadarParams(json::value params){
            s_params = params;
        }

    private:
        static json::value s_params;
        vector<int> m_path; //路径
        vector<int> m_last_pulse_idx; //上一个脉冲的位置
        double m_likelihood; // 似然函数值   

        vector<int> m_t_index; // for sinu

        vector<vector<double>> m_alpha; // for stagger

        // for sliding 
        vector<double> m_last_pri;// 上一次的pri
        vector<int> m_sliding_period; // 滑步周期
};
using PathPtr = shared_ptr<Path>;
