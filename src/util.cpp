#include "util.h"
#include <unistd.h>
#include <string>
using namespace std;
std::string get_cwd()
{
    const int MAXPATH=250;
    char buffer[MAXPATH];
    getcwd(buffer, MAXPATH);
    string cwd = buffer;
    return cwd;
}