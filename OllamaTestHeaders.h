#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "httplib.h"
#include "json.hpp"
#include <cstdio>
#include <map> 

using namespace std;
using json = nlohmann::json;




extern map <string,string> settings;
extern bool recording;            // ← тут
extern string greeting;      // ← тут
extern string textgreeting;  // ← тут
extern vector<json> history ;// ← тут
extern json tools;
extern string uinput;
extern string uianswer;
extern string syspromptfile;
extern string sysprompt;
extern string filepath;


void startRec();
void stopRec();

string readSysPrompt(const string& syspromptfile);
void readSettings();

bool SendToOllama();
string SendToWhisper();
string getCmd();

string runTool(const string& name, const json& args);
string executeCommand(const string& cmd);
