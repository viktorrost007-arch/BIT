#include "server_headers.h"

string OLLAMA_HOST = "http://localhost:11434";
string OLLAMA_MODEL = "qwen2.5:7b";
int SERVER_PORT = 8080;
vector<json> history;
json tools;

int main() {
    // Чтение из Docker
    if (getenv("OLLAMA_HOST")) OLLAMA_HOST = getenv("OLLAMA_HOST");
    if (getenv("OLLAMA_MODEL")) OLLAMA_MODEL = getenv("OLLAMA_MODEL");
    if (getenv("SERVER_PORT")) SERVER_PORT = stoi(getenv("SERVER_PORT"));

    // Загрузка инструментов
    ifstream f("tools.json");
    if (f.is_open()) f >> tools;

    // Запуск WebSocket в фоне
    thread ws(start_websocket_server);
    ws.detach();

    // Главный цикл (чат)
    string uinput;
    while (getline(cin, uinput)) {
        if (uinput == "exit") break;
        history.push_back({ {"role", "user"}, {"content", uinput} });
        while (SendToOllama()) { }
    }
    return 0;
}