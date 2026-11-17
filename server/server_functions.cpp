#include "server_headers.h"

httplib::ws::WebSocket* laptop_ws = nullptr;
httplib::ws::WebSocket* speaker_ws = nullptr;
bool busy = false;
string reply = "";

void start_websocket_server() {
    httplib::Server svr;

    // Ноут
    svr.WebSocket("/laptop", [](const httplib::Request&, httplib::ws::WebSocket& ws) {
        laptop_ws = &ws;
        string msg;
        while (ws.read(msg)) {
            reply = json::parse(msg)["output"];
            busy = false;
        }
        laptop_ws = nullptr;
    });

      // Колонка (ESP32)
    svr.WebSocket("/speaker", [](const httplib::Request&, httplib::ws::WebSocket& ws) {
        speaker_ws = &ws;
        string msg;
        while (ws.read(msg)) {
            // 1. Колонка прислала WAV. Сохраняем.
            ofstream f("/tmp/input.wav", ios::binary);
            f.write(msg.data(), msg.size());
            f.close();

            // 2. Распознаем текст
            string text = sendToWhisper("/tmp/input.wav");
            if (text.empty()) continue;

            // 3. Думаем
            history.push_back({ {"role", "user"}, {"content", text} });
            while (SendToOllama()) { }

            // 4. Получаем ответ и генерируем WAV
            string answer = history.back()["content"];
            string audio_bytes = generateAudio(answer);
            
            // 5. Отправляем аудио обратно
            ws.send(audio_bytes); 
        }
        speaker_ws = nullptr;
    });

    svr.listen("0.0.0.0", SERVER_PORT);
}

string call_laptop(const string& command) {
    if (!laptop_ws) return "[Error] Laptop offline";
    busy = true; reply = "";
    laptop_ws->send(json{{"command", command}}.dump());
    while (busy) this_thread::sleep_for(chrono::milliseconds(100));
    return reply;
}

bool SendToOllama() {
    json req = {
        {"model", OLLAMA_MODEL},
        {"stream", false},
        {"tools", tools},
        {"messages", history}
    };

    httplib::Client cli(OLLAMA_HOST);
    auto res = cli.Post("/api/chat", req.dump(), "application/json");
    if (!res || res->status != 200) return false;

    json msg = json::parse(res->body)["message"];
    history.push_back(msg);

    if (msg.contains("tool_calls") && !msg["tool_calls"].empty()) {
        for (auto &call : msg["tool_calls"]) {
            string name = call["function"]["name"];
            json args = call["function"]["arguments"];
            if (args.is_string()) args = json::parse(args.get<string>());

            string out = runTool(name, args);
            
            history.push_back({
                {"role", "tool"},
                {"tool_name", name},
                {"content", out}
            });
        }
        return true;
    }
    return false;
}


// Отправка файла в Whisper
string sendToWhisper(const string& filepath) {
    string cmd = "curl -s http://whisper:10300/v1/audio/transcriptions -F \"file=@" + filepath + "\"";
    FILE* p = popen(cmd.c_str(), "r");
    if (!p) return "";
    string out; char buf[4096];
    while (fgets(buf, sizeof(buf), p)) out += buf;
    pclose(p);
    try {
        return json::parse(out).value("text", "");
    } catch (...) { return ""; }
}

// Генерация аудио через Piper (возвращает сырые байты WAV)
string generateAudio(const string& text) {
    ofstream("/tmp/say.txt") << text;
    system("./piper/piper -m ./piper/ru.onnx -f /tmp/answer.wav < /tmp/say.txt 2>/dev/null");
    
    ifstream f("/tmp/answer.wav", ios::binary);
    stringstream ss; ss << f.rdbuf();
    return ss.str();
}