#include <iostream>
#include <unistd.h>
#include <filesystem>
#include "util.h"
using namespace std;
int main(int argc, char** argv)
{
    // const int MAXPATH=250;
    // char buffer[MAXPATH];
    // getcwd(buffer, MAXPATH);
    // printf("The current directory is: %s", buffer);
    std::filesystem::path path(get_cwd());
    std::filesystem::path dataPath = path.parent_path() / "data/";
    cout << dataPath;
}
