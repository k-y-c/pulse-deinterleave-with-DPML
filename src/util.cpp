#include "util.h"
#include <unistd.h>
#include <string>
#include <math.h>
#include <iostream>
#include <limits.h>
#include <cstring>
using namespace std;

std::string get_cwd()
{
    const int MAXPATH=250;
    char buffer[MAXPATH];
    getcwd(buffer, MAXPATH);
    string cwd = buffer;
    return cwd;
}

void printUsage(const std::string& programName) {
    std::cerr << "Usage: " << programName << " -c config_file (default:../config/conf.json)" << std::endl;
}

void parse_args(int argc,char* argv[],string & conf_file)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c" && i + 1 < argc) {
            conf_file = argv[++i];
        }
        else
        {
            printUsage(argv[0]);
            exit(1);
        }
    }
}

int chcwd()
{
    // 获取当前执行文件的路径
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);

    if (count != -1) {
        buffer[count] = '\0';  // 添加字符串终止符
        // 获取当前执行文件所在的目录
        char* lastSlash = strrchr(buffer, '/');
        if (lastSlash != NULL) {
            *lastSlash = '\0';  // 截断字符串，保留目录部分

            // 将当前工作路径修改为执行文件所在的目录
            if (chdir(buffer) == 0) {
                std::cout << "Current working directory changed to: " << buffer << std::endl;
            } else {
                std::cerr << "Error changing current working directory" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error parsing file directory" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error getting executable path" << std::endl;
        return 1;
    }

    // 现在，当前工作路径已经被修改为可执行文件所在的路径

    return 0;
}


double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}