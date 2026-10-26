#pragma once

#include <iostream>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
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

extern const string VERSION;


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
extern bool autohealing;
extern bool first_time;
extern string greeting2;
extern string greeting3;
extern string point;
extern bool allowcommand;
extern bool is_thinking;
extern map <string, string> colors;
extern bool textgreeting2_enabled;
extern string piper_command;


void reset();
void checkfirst_time();
void setup();
void setup_RU();
void setup_DE();
void myexit();
void update();

void handleInput();
bool checkUserInput();
void startRec();
void stopRec();
void say(const string& soutnput);

string readSysPrompt(const string& syspromptfile);
void readSettings();

bool SendToOllama();
void trimHistory();
string SendToWhisper();
string getCmd();

string runTool(const string& name, const json& args);
string executeCommand(const string& cmd);

/*
твоя_папка_с_проектом/
├── ollamatest
├── settings.txt
└── piper/                         <-- Папка с Piper
    ├── piper                      <-- Программа
    ├── ru_RU-ruslan-medium.onnx   <-- Сама модель (ЭТО НУЖНО УКАЗАТЬ)
    └── ru_RU-ruslan-medium.onnx.json\
    */