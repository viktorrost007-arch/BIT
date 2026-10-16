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

* WELCOME TO THE BETA VERSION OF BIT, PLEASE FOLLOW INSTRUCTIONS BELOW.
* FOR EXIT PLEASE USE /exit COMMAND INSTEAD OF Ctrl+C, THIS WILL HELP KEEP YOUR BIT STABLE
* FOR SETUP PLEASE USE /setup COMMAND.
* FOR RESETING LOCAL SETTINGS PLEASE USE /reset COMMAND, THIS WILL START SETUP AGAIN.
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

void reset(){
    cout<< endl<< "Reseting local settings";
    system("clear");
    remove("settings.txt");
    cout<< ".";
    remove("prompt.txt");
    cout<< "." << endl;
    setup();
}
void checkfirst_time(){
    ifstream file("settings.txt");
    if(!file.is_open()){
        first_time = true;
        setup();
    }
     
}

void setup(){
    map <string, string> first_time_settings;
    
    textgreeting = "Hi, I am BIT, nice to meet you here! Please fill the settings and we can start! " ;
    
     ofstream("/tmp/say.txt") << textgreeting;
    system("espeak-ng -v en -f /tmp/say.txt");


    cout<< "Hi, I am BIT, nice to meet you here! Please fill out the settings and we can start! " << endl;
    cout<< "You can change them every time you want in settings.txt"<< endl;
    cout << "Please write your system-prompt in prompt.txt"<< endl << endl;
    

    cout << "Which art of LLM do you use? (ollama for Ollama server, openai for Deepseek/ChatGPT/Claude and others)" << endl;
    cin >> first_time_settings["llm_type"];

    if(first_time_settings["llm_type"] == "openai"){
        cout << endl << "Where do I have to search your OpenAI? (for example https://api.openai.com  (WITHOUT /v1/chat/completions !))" << endl;
        cin >> first_time_settings["openai_address"];

        cout << endl << "Which LLM model do you use in OpenAI? (for example gpt-4o, gpt-4o-mini, gpt-4, gpt-3.5-turbo)" << endl;
        cin >> first_time_settings["openai_model"];

        cout << endl << "Please enter your OpenAI API key (for example sk-xxxxxxxxxxxxxxxxxxxxxx)" << endl;
        cin >> first_time_settings["llm_key"];
    }

    if(first_time_settings["llm_type"] == "ollama"){
        cout << endl << "Where do I have to search your Ollama? (for example 192.168.20.4:11434)" << endl;
        cin >> first_time_settings["ollama_address"];
        cout << endl << "Which LLM model do you use in Ollama? (for example qwen2.5:7b)" << endl;
        cin >> first_time_settings["ollama_model"];
    }

    cout << endl << "Where do I have to search your Whisper.cpp? (for example http://192.168.20.4:10300/v1/audio/transcriptions)" << endl;
    cin >> first_time_settings["whisper_address"];
    

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
   
   
// считываем параметры моделей (условие да: нет

   bool isOllama = (settings["llm_type"] == "ollama");

    string host  = isOllama ? settings["ollama_address"] : settings["openai_address"];
    string path  = isOllama ? "/api/chat"             : "/v1/chat/completions";
    string model = isOllama ? settings["ollama_model"] : settings["openai_model"];


    json request = {
        {"model",    model},
        {"stream",   false},
        {"tools",    tools},
        {"messages", history}
    };


    httplib::Client cli(host);
    cli.set_read_timeout(60, 0);
    if (!isOllama) cli.set_bearer_token_auth(settings["llm_key"]);

    for(int i = 0; i<3; i++){
        cout << ".";
        cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    auto res = cli.Post(path, request.dump(), "application/json");
    cout << " \r               " << flush;
     

    if (!res) { cerr << "http error\n"; return false; }

    if (res->status != 200) {
    cerr << "LLM error " << res->status << ": " << res->body << "\n";
    return false;
    }

    if (res->body.empty()) { cerr << "empty body\n"; return false; }


    json msg; // парсим ответ в зависимости от типа LLM (Ollama или OpenAI)
    if (isOllama) {
        msg = json::parse(res->body)["message"];
    } else {
        msg = json::parse(res->body)["choices"][0]["message"];
    }   


    history.push_back(msg);

    if(msg.contains("tool_calls") && !msg["tool_calls"].empty()){

        for(auto& call : msg["tool_calls"]){
            string name = call["function"]["name"];
            json args = call ["function"]["arguments"];
            args = args.is_string() ? json::parse(args.get<string>()) : args;
            string toolout = runTool(name,args);
            //cout<< name << ">> "<< args;
            history.push_back({
                {"role", "tool"},
                {"tool_name", name},
                {"content", toolout}
            });
            cout << "> TOOL"<< " "<< name << endl;
            cout << "> "<< args << endl;
            
            ofstream log("log.txt", ios::app);
            time_t now = time(0);
            log << "TIME: " << ctime(&now);
            log << "TOOL: " << name << "\n";
            log << "ARGS: " << args.dump(2) << "\n";
            log << "OUT:  " << toolout << "\n\n";
            
            //log.close();
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