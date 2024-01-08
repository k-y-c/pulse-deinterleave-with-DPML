#include "util.h"
#include <unistd.h>
#include <string>
#include <math.h>
using namespace std;

std::string get_cwd()
{
    const int MAXPATH=250;
    char buffer[MAXPATH];
    getcwd(buffer, MAXPATH);
    string cwd = buffer;
    return cwd;
}

double gaussian_distribution(double mu,double sigma,double value)
{
    double fx = (1.0/(sqrt(2*M_PI)*sigma))*exp(-pow(value-mu,2)/(2*sigma*sigma));
    return fx + 1e-5;
}