#include "OllamaTestHeaders.h"

using namespace std;
using json = nlohmann::json;

string uinput;
string uianswer;

bool recording = false;

string greeting = R"(
██████╗ ██╗████████╗
██╔══██╗██║╚══██╔══╝
██████╔╝██║   ██║
██╔══██╗██║   ██║
██████╔╝██║   ██║  v.1.beta
╚═════╝ ╚═╝   ╚═╝
)";

string textgreeting = " nice to see you again!";

vector<json> history;
// request headers
json tools = json::parse(ifstream("tools.json"));



string source = "192.168.20.4:11434";

string syspromptfile = "prompt.txt";
string sysprompt = readSysPrompt(syspromptfile);
string filepath = "test.wav";

int counter;

FILE* rec = nullptr;






void startRec() {
    rec = popen("arecord -f S16_LE -r 16000 -c 1 -t wav test.wav -q", "w");
}

void stopRec() {
    system("pkill -INT arecord");
    if (rec) { pclose(rec); rec = nullptr; }
}

//////sysprompt

string readSysPrompt(const string& syspromptfile){
    ifstream file(syspromptfile);

    if(!file.is_open()) {
        std::cerr << "CAN'T OPEN"<< syspromptfile << endl;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


////send function

bool SendToOllama() {

    json request = {
        {"model",  "qwen2.5:7b"},
        {"stream", false},
        {"tools", tools},
        {"messages", history}
    };

    // 1. Указываем только БАЗОВЫЙ адрес сервера (куда стучимся по сети)
    httplib::Client cli("http://192.168.20.4:11434");
    cli.set_read_timeout(60, 0);

    auto res = cli.Post("/api/chat", request.dump(), "application/json");
    if (!res) { cerr << "http error\n"; return false; }

   
    json msg = json::parse(res->body)["message"];
    history.push_back(msg);

    if(msg.contains("tools_calls") && !msg["tool_calls"].empty()){
        for(auto& call : msg["tool_calls"]){
            string name = call["function"]["name"];
            json args = call ["function"]["arguments"];
            string toolout = runTool(name,args);
            cout<< name << ">> "<< args;
            history.push_back({
                {"role", "tool"},
                {"tool_name", name},
                {"content", toolout}
            });
        }
        return true;
    }
    
    uianswer = msg["content"];
    return false;

}






string SendToWhisper() {
    string cmd = "curl -s http://192.168.20.4:10300/v1/audio/transcriptions -F \"file=@test.wav\"";

    FILE* p = popen(cmd.c_str(), "r");
    if (!p) return "";

    string out;
    char buf[4096];
    while (fgets(buf, sizeof(buf), p)) out += buf;
    pclose(p);

    return json::parse(out).value("text", "");
}



// string getCmd() {
//     size_t a = uianswer.find("<cmd>");
//     size_t b = uianswer.find("</cmd>");
//     if (a == string::npos || b == string::npos) return "";
//     return uianswer.substr(a + 5, b - a - 5);
//
// }

string runTool(const string& name, const json& args) {
    if (name == "run_shell") return executeCommand(args["command"]);
    return "unknown tool: " + name;
}


string executeCommand(const string& cmd) {
    FILE* p = popen((cmd + " 2>&1").c_str(), "r");
    if (!p) return "[popen failed]";

    string out;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), p)) > 0) {
        out.append(buf, n);
    }
    pclose(p);

    if (out.empty()) out = "[no output]";
    if (out.size() > 4000) out = out.substr(0, 4000) + "\n...(truncated)";
    return out;
}
