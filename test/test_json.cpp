#include "configor/json.hpp"
#include <iostream>
#include <fstream>


using namespace configor;
using namespace std;
int main(){
    json::value j;
    string file = "config/conf.json";
    ifstream ifs(file);
    ifs >> json::wrap(j);
    cout << j["glossary"]["title"].get<string>();
    return 0;
}