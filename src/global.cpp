#include <filesystem>
#include "global.h"
#include "dpml.h"
#include "util.h"

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

vector<Path> paths;//所有路径

void parse_args(int argc,char* argv[]){
    if(argc>1){
        // beta_ = atoi(argv[1]);
        MODULATION = string(argv[1]);
        RATE = string(argv[2]);
    }
}

//读取toa、label数据

void read_data()
{
    std::filesystem::path path(get_cwd());
    // 获取上一个目录
    std::filesystem::path dataPath = path.parent_path() / "data/";
    const string DATA_PATH = dataPath.string()+TYPE+MODULATION+"/"+RATE+"/";
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