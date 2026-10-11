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

string textgreeting;

vector<json> history;
// request headers
json tools = json::parse(ifstream("tools.json"));



//string source = "192.168.20.4:11434";

string syspromptfile = "prompt.txt";
string sysprompt = readSysPrompt(syspromptfile);
string filepath = "test.wav";

int counter;

FILE* rec = nullptr;
  
map <string, string> settings;

bool autohealing = false;
bool first_time;

void checkfirst_time(){
    ifstream file("test.wav");
    if(!file.is_open()){
        first_time = true;
        setup();
    }
     
}

void setup(){
    map <string, string> first_time_settings;
    
    textgreeting = "Hi, I am BIT, nice to meet you here! Please fill the settings and we can start! " ;
    cout<< "Hi, I am BIT, nice to meet you here! Please fill out the settings and we can start! " << endl;
    cout<< "You can change them every time you want in settings.txt"<< endl;
    cout << "Please write your system-prompt in prompt.txt"<< endl << endl;
    cout<< " Which LLM model do you use in Ollama? (for example qwen2.5:7b)"<< endl;
    cin >> first_time_settings["ollama_model"];
    
    cout << endl << "Where do I have to search your Whisper.cpp? (for example http://192.168.20.4:10300/v1/audio/transcriptions)" << endl;
    cin >> first_time_settings["whisper_address"];
    
    cout << endl << "Where do I have to search your Ollama? ( for example 192.168.20.4:11434)"<< endl;
    cin >> first_time_settings["ollama_address"];

    cin.ignore(); // сбросил остающийся \n
    cout << endl << "Which greeeting do you want to set for me?"<< endl;
    getline(cin,first_time_settings["greeting"]);

    ofstream file("settings.txt");
    file << "*WELCOME IN THE BETA VERSION OF THE SETTINGS-FILE, PLEASE FOLLOW INSTRUCTIONS BELOW.\n";
    file << "*please dont use any whitespaces as the first or last symbol of the line,\n";
    file << "*please also don't change the values after hashtags.\n";
    file << "*comments are marked with * at the beginning of the line, you can add there whatever you want\n\n";

    for (auto& [key, value] : first_time_settings) {
    file << "#" << key << "\n" << value << "\n\n";
     }
     first_time = false;
}




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
    cout<< "Sending to LLM";
    json request = {
        {"model",  settings["ollama_model"]},
        {"stream", false},
        {"tools", tools},
        {"messages", history}
    };
    string cliadress = settings["ollama_address"];
    // 1. Указываем только БАЗОВЫЙ адрес сервера (куда стучимся по сети)
    httplib::Client cli(cliadress);
    cli.set_read_timeout(60, 0);

    auto res = cli.Post("/api/chat", request.dump(), "application/json");
    cout << " \r               " << flush; 
    if (!res) { cerr << "http error\n"; return false; }

   
    json msg = json::parse(res->body)["message"];
    history.push_back(msg);

    if(msg.contains("tool_calls") && !msg["tool_calls"].empty()){
        for(auto& call : msg["tool_calls"]){
            string name = call["function"]["name"];
            json args = call ["function"]["arguments"];
            string toolout = runTool(name,args);
            //cout<< name << ">> "<< args;
            history.push_back({
                {"role", "tool"},
                {"tool_name", name},
                {"content", toolout}
            });
            cout << "> TOOL"<< " "<< name << endl;
            cout << "> "<< args << endl;
            
        }
        return true;
    }
    
    uianswer = msg["content"];
    
    return false;

}






string SendToWhisper() {
    //string cmd = "curl -s http://192.168.20.4:10300/v1/audio/transcriptions -F \"file=@test.wav\"";
    string cmd = "curl -s " + settings["whisper_address"] + " -F \"file=@test.wav\"";
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

void readSettings(){

    string line, key;
    ifstream file("settings.txt");
    if(!file.is_open()) {
        std::cerr << "CAN'T OPEN settings.txt"<< endl;
        cout << "There are some sources missing. Start auto-healing?" << endl;
        autohealing = true; 
        cout << "Sorry, auto-healing not suppoorted in this version, please enter setup or create file settings.txt at yourself"<< endl;
        cout << "Enter setup? y for yes, n for no"<< endl;
        string i;
        cin >> i;
        if(i == "y" || i == "Y") setup();
        else {
            cout << "You can create settings.txt at yourself, please follow the instructions in the file." << endl;
            cout << "Please also create prompt.txt with your system-prompt." << endl;
            cout << "Exiting..." << endl;
            exit(1);
        }


        return;
        }
    while(getline(file, line)){
        if (line.empty() || line[0] == '*') continue; // skip empty lines 
        if (line[0] == '#') {key = line.substr(1); continue;}
        settings[key] = line;
        
        }
    }