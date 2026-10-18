#include "OllamaTestHeaders.h"

const string VERSION = "1.0.0";


using namespace std;
using json = nlohmann::json;

string uinput;
string uianswer;

bool recording = false;
bool is_thinking = false;
     
string greeting = R"(
██████╗ ██╗████████╗
██╔══██╗██║╚══██╔══╝
██████╔╝██║   ██║
██╔══██╗██║   ██║
██████╔╝██║   ██║  v.1.beta
╚═════╝ ╚═╝   ╚═╝

)";

string greeting2 = 
    "\n* WELCOME TO THE BETA VERSION OF BIT, PLEASE FOLLOW INSTRUCTIONS BELOW.\n\n"
    "* FOR EXIT PLEASE USE \033[33m/exit\033[0m COMMAND INSTEAD OF Ctrl+C, THIS WILL HELP KEEP YOUR BIT STABLE\n"
    "* FOR SETUP PLEASE USE \033[33m/setup\033[0m COMMAND.\n"
    "* FOR RESETING LOCAL SETTINGS PLEASE USE \033[33m/reset\033[0m COMMAND, THIS WILL START SETUP AGAIN.\n"
    "* UPDATE WITH \033[33m/update\033[0m . THIS WILL RUN update.sh .\n"
    "* UPDATE FROM LAST COMMIT WITH \033[33m/update-from-repo\033[0m . THIS WILL RUN update-from-repo.sh . This version is not production ready, I dont recommend to do this.\n"
    "* ALLOW EXECUTING COMMANDS WITHOUT ASKING WITH \033[33m/allow\033[0m, DISALLOW WITH \033[33m/disallow\033[0m.\n"
    "* PRESS ENTER TO START RECORDING, PRESS ENTER AGAIN TO STOP RECORDING AND SEND AUDIO TO BIT.\n"
    "* FOR HELP PLEASE USE \033[33m/help\033[0m COMMAND.\n";

string point = "  ██|";

map<string, string> colors = {
    {"RED", "\033[31m"},
    {"GREEN", "\033[32m"},
    {"YELLOW", "\033[33m"},
    {"BLUE", "\033[34m"},
    {"RESET", "\033[0m"}
};

//colors.insert({"RED", "\033[31m"}); или emplace НО ТОЛЬКО ВНУТРИ ФУНКЦИЙ!

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
bool allowcommand = false;









// bool updateAvailable() {
//     httplib::Client cli("https://raw.githubusercontent.com");
//     cli.set_read_timeout(10, 0);
//     auto res = cli.Get("/bit-llm/bit/main/VERSION.txt");
//     if (!res || res->status != 200) {
//         cerr << "Failed to check for updates\n";
//         return false;
//     }
//     string latestVersion = res->body;
//     return latestVersion != VERSION;
// }

void update(){
    myexit();
    cout<< "Start updating."<< endl;
    system("chmod +x update.sh");
    cout << ".";
    system("clear");
    cout << "Updating..."<< endl;
    system("./update.sh");
    
}



void myexit(){
  cout << colors["RED"] << "Exiting." << colors["RESET"] << endl;
  remove("test.wav");
  cout << ".";
  remove("/tmp/say.txt");
  cout << "."<< endl; 
  remove("log.txt");
  cout << "Done."<< endl;
  cout << "Goodbye!"<< endl;
  system("espeak-ng -v en \"Meet you later!\"");

  system("clear");
}



void reset(){
    cout << colors["RED"]<< "DO YOU REALLY WANT TO DELTE ALL YOUR PERSONAL SETTINGS?" << colors["YELLOW"] << " (y/n) " << colors["RESET"] << endl;
    string tmp;
    cin >> tmp;
    if(tmp == "y" || tmp == "Y"){
    cout<< endl<< "Reseting local settings";
    system("clear");
    remove("settings.txt");
    cout<< ".";
    remove("prompt.txt");
    cout<< "." << endl;
    setup();
    }
    else {
        cout << "Reset canceled." << endl;
    }
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
    
    cout<< colors["GREEN"]<< "Hi, I am BIT, nice to meet you here! Please fill out the settings and we can start! "<< colors["RESET"] << endl;
    cout<< "You can change them every time you want in settings.txt"<< endl;
    cout << "Please write your system-prompt in prompt.txt"<< endl << endl;


     ofstream("/tmp/say.txt") << textgreeting;
     system("espeak-ng -v en -f /tmp/say.txt");

    

    cout << "Which art of LLM do you use? ("<< colors["YELLOW"] << "ollama"<< colors["RESET"] << " for Ollama server, " << colors["YELLOW"] << "openai" << colors["RESET"] << " for Deepseek/ChatGPT/Claude and others)" << endl;
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
    cout << endl << "Which greeeting do you want to set for me? (I will say it every time you will start me)"<< endl;
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





void handleInput() {
    while (getline(cin, uinput)) {

  //cout << uinput<< endl;
 
   if(uinput == "/exit") myexit(); 
   if( uinput == "/setup") setup();
   if( uinput == "/reset") reset();
   if ( uinput == "/update")  update();
   if ( uinput == "/allow")  {
       allowcommand = true;
       cout << colors["YELLOW"] << "All commands are allowed now" << colors["RESET"] << endl;
       uinput = "";
       continue;
       //uinput = "go on you are allowed to do all commands without asking, dont forget to explain what you did when you are done and be carefull with users system. ";
   }
   if ( uinput == "/disallow")  {
       allowcommand = false;
        cout << colors["GREEN"] << "All commands are disallowed now, BIT will ask you to execute them" << colors["RESET"] << endl;
        uinput ="";
        continue;
        //uinput = "executing commands without asking is not allowed now, please ask the user for allowing it, explain why you need it";
   }
   if (uinput == "/update-from-repo") {
        system("chmod +x update-from-repo.sh");
        system("./update-from-repo.sh");
        myexit();
   }
   if( uinput == "/help") {
       cout << colors["YELLOW"] << "Available commands:\n"
            << "/exit - exit the program\n"
            << "/setup - run setup again\n"
            << "/reset - reset local settings and run setup again\n"
            << "/update - update the program from the release\n"
            << "/update-from-repo - update from the last commit\n"
            << "/allow - allow executing commands without asking\n"
            << "/disallow - disallow executing commands without asking\n"
            << "/help - show this help message" << colors["RESET"] << endl;
        uinput = "";
        continue;
   }

    if (uinput.empty()) {
      if (!recording) {
        startRec();
        recording = true;
        cout << "rec...\n";
        continue;                      // ← выходим, ничего больше не делаем
      } else {
        stopRec();
        //cout << "\r               " << flush << endl << endl;
        recording = false;

        string uinput = SendToWhisper();
        cout << "you> " << uinput  << "\n";
        if (uinput.empty()) continue;

        history.push_back({ {"role","user"}, {"content", uinput} });
        tcflush(STDIN_FILENO, TCIFLUSH);
        is_thinking = true;
        while(SendToOllama()&& !checkUserInput()){}
        is_thinking = false;
      }
    } else {
      history.push_back({ {"role","user"}, {"content", uinput} });
       tcflush(STDIN_FILENO, TCIFLUSH);
        is_thinking = true;
        while(SendToOllama() ){
            
            if (checkUserInput() ) {
                cout << colors["RED"] << "[Interrupted by User]" << colors["RESET"] << "\n";
                //uianswer = "";
                break;
            }
        }
        is_thinking = false;
        
        
    }
    
    //cout << "BIT> \"" <<uianswer << """" << "\"\n";
    cout << colors["BLUE"] << "BIT>  "  << colors["RESET"] << uianswer  << endl;
    // string speechcommand = "espeak-ng -v ru \"" + uianswer + "\"";
    // system(speechcommand.c_str());
    ofstream("/tmp/say.txt") << uianswer;
    system("espeak-ng -v ru -f /tmp/say.txt");

  
}
}


bool checkUserInput() {
    if(!is_thinking) return false; 
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds); // Следим за клавиатурой (дескриптор 0)

    // Ставим таймаут 0 секунд 0 микросекунд (проверить прямо сейчас, не ждать)
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // select возвращает > 0, если в буфере есть данные
    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
    
    if (result > 0) {
        tcflush(STDIN_FILENO, TCIFLUSH);
        return true; // Пользователь что-то нажал!
    }
    return false; // Ввод пуст
}


//////sysprompt

string readSysPrompt(const string& syspromptfile){
    ifstream file(syspromptfile);

    if(!file.is_open()) {
        std::cerr << colors ["RED"] << "CAN'T OPEN"<< syspromptfile << colors["RESET"] << endl;
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
    cout << "Thinking"<< endl;
    for(int i = 0; i<3; i++){
        cout << ".";
        cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    auto res = cli.Post(path, request.dump(), "application/json");
    cout << " \r               " << flush;
     

    if (!res) { cerr << "http error\n"; return false; }

    if (res->status != 200) {
    cerr<< colors["RED"] << "LLM error " << res->status << ": " << res->body << "\n" << colors["RESET"];
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


            cout<< colors["BLUE"]<< "> TOOL"<< " "<< name<< colors["RESET"] << endl;
            cout << "> "<< args << endl;

            if(!allowcommand){
                cout<< endl<< colors["YELLOW"] << "ALLOW THIS COMMAND? (y/n) "<< colors["RESET"] << endl;
                string answer;
                getline(cin, answer);
                if(answer != "y" && answer != "Y"){
                    cout << "Command not allowed, skipping..." << endl;
                    continue;
                }
            }
                
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
            
            
            
       
    }   //for
    return true;

}// message contains
    
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
            cout << "Please also create prompt.txt for your system-prompt." << endl;
            cout << "Exiting..." << endl;
            system("clear");
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