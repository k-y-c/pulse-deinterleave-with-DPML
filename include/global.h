#ifndef GLOBAL
#define GLOBAL

#include <vector>
#include <string>


#define GAUSSIAN1 1
#define GAUSSIAN2 2

#define _USE_MATH_DEFINES  //pi
using namespace std;

extern const int K; // 辐射源数目
extern const double T_K; //超时时间（单位：ns）
extern vector<double> toa; // toa数据
extern vector<int> label; // 数据标签
extern const double beta_; // 似然因子
extern const int buffer_size; //路径buffer大小
extern const bool DEAL_MISS; // 修正漏脉似然因子
extern const string TYPE; //数据类型
// const string TYPE = "spurious_";
extern string MODULATION; //调制类型
extern string RATE; // 率

extern const string DATA_FILE_NAME;
extern const string LABEL_FILE_NAME;


extern vector<vector<double>> param_jitter;  //3个辐射源的jitter分布参数

extern vector<vector<double>> param_sliding;  //3个辐射源的sliding分布参数

extern vector<vector<double>> param_stagger;  // stagger分布参数
extern double stagger_sigma;  // stagger分布参数

extern vector<vector<double>> param_sinu;  // sinu分布参数

class Path;
extern vector<Path> paths;//所有路径

void parse_args(int argc, char* argv[]);

void read_data();

#endif