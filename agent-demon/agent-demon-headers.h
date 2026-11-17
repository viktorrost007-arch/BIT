#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;



string runTool(const string &name, const json &args);
string executeCommand(const string &cmd);