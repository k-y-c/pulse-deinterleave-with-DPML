#include <string>
using namespace std;
string get_cwd();

void printUsage(const string& programName);

void parse_args(int argc,char* argv[],string & conf_file);

int chcwd();

double gaussian_distribution(double mu,double sigma,double value);