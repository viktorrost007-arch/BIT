#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Настройки
extern string OLLAMA_HOST;
extern string OLLAMA_MODEL;
extern int SERVER_PORT;
extern vector<json> history;
extern json tools;

// Сокеты устройств
extern httplib::ws::WebSocket* laptop_ws;
extern httplib::ws::WebSocket* speaker_ws;

// Функции
void start_websocket_server();
string call_laptop(const string& command);
bool SendToOllama();

string runTool(const string& name, const json& args);
string executeCommand(const string& cmd);
string sendToWhisper(const string& filepath);
string generateAudio(const string& text);