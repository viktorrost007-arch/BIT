
#include <OllamaTestHeaders.h>

void setup_RU() {   
    uinput = "";
    map<string, string> first_time_settings;

    textgreeting = "Привет, я BIT, рад познакомиться! Пожалуйста, заполни настройки, и мы начнем! ";

    cout << colors["GREEN"] << "Привет, я BIT, рад познакомиться! Пожалуйста, заполни настройки, и мы начнем! " << colors["RESET"] << endl;
    cout << "Ты можешь изменить их в любое время в файле settings.txt" << endl;
    cout << "Пожалуйста, напиши свой системный промпт в файле prompt.txt" << endl << endl;

    ofstream("/tmp/say.txt") << textgreeting;
    system("espeak-ng -v ru -f /tmp/say.txt");

    cout << "Какой тип LLM ты используешь? (" << colors["YELLOW"] << "ollama" << colors["RESET"] << " для сервера Ollama, " << colors["YELLOW"] << "openai" << colors["RESET"] << " для Deepseek/ChatGPT/Claude и других)" << endl;
    cin >> first_time_settings["llm_type"];

    if (first_time_settings["llm_type"] == "openai") {
        cout << endl << "Где мне искать твой OpenAI? (например https://api.openai.com (БЕЗ /v1/chat/completions !))" << endl;
        cin >> first_time_settings["openai_address"];

        cout << endl << "Какую модель LLM ты используешь в OpenAI? (например gpt-4o, gpt-4o-mini, gpt-4, gpt-3.5-turbo)" << endl;
        cin >> first_time_settings["openai_model"];

        cout << endl << "Пожалуйста, введи свой API ключ OpenAI (например sk-xxxxxxxxxxxxxxxxxxxxxx)" << endl;
        cin >> first_time_settings["llm_key"];
    }

    if (first_time_settings["llm_type"] == "ollama") {
        cout << endl << "Где мне искать твой Ollama? (например 192.168.20.4:11434)" << endl;
        cin >> first_time_settings["ollama_address"];
        cout << endl << "Какую модель LLM ты используешь в Ollama? (например qwen2.5:7b)" << endl;
        cin >> first_time_settings["ollama_model"];
    }

    cout << endl << "Где мне искать твой Whisper.cpp? (например http://192.168.20.4:10300/v1/audio/transcriptions)" << endl;
    cin >> first_time_settings["whisper_address"];

    cout << "Это твой первый раз, когда ты используешь BIT? (y/n)" << endl;
    string answer;
    cin >> answer;
    if (answer == "y" || answer == "Y")
        textgreeting2_enabled = true;
    else
        textgreeting2_enabled = false;

    cin.ignore(); // сбросили остающийся \n
    cout << endl << "Какое приветствие ты хочешь мне задать? (Я буду говорить его при каждом твоем запуске)" << endl;
    getline(cin, first_time_settings["greeting"]);

    ofstream file("settings.txt");
    file << "*ДОБРО ПОЖАЛОВАТЬ В БЕТА-ВЕРСИЮ ФАЙЛА НАСТРОЕК, ПОЖАЛУЙСТА, СЛЕДУЙТЕ ИНСТРУКЦИЯМ НИЖЕ.\n";
    file << "*пожалуйста, не используйте пробелы в качестве первого или последнего символа строки,\n";
    file << "*пожалуйста, также не меняйте значения после решеток.\n";
    file << "*комментарии помечены * в начале строки, вы можете добавить туда все, что захотите\n\n";

   file << "#piper_model_path_ru \n ./piper/ru_RU-ruslan-medium.onnx \n\n";
    file << "#piper_model_path_de \n ./piper/de_DE-thorsten-medium.onnx \n\n";
    file << "#piper_model_path_en \n ./piper/en_US-lessac-medium.onnx \n\n";
    
    for (auto &[key, value] : first_time_settings)
    {
        file << "#" << key << "\n"
             << value << "\n\n";
    }
    if(first_time_settings["language"]== "RU")
    file << "#piper_model \n " << first_time_settings["piper_model_path_ru"] << "\n\n";

    if(first_time_settings["language"]== "DE")
    file << "#piper_model \n " << first_time_settings["piper_model_path_de"] << "\n\n";

    if(first_time_settings["language"]== "EN")
    file << "#piper_model \n " << first_time_settings["piper_model_path_en"] << "\n\n";
    first_time = false;
}




void setup_DE() {   
    uinput = "";
    map<string, string> first_time_settings;

    textgreeting = "Hallo, ich bin BIT, schön dich hier zu treffen! Bitte fülle die Einstellungen aus, dann können wir starten! ";

    cout << colors["GREEN"] << "Hallo, ich bin BIT, schön dich hier zu treffen! Bitte fülle die Einstellungen aus, dann können wir starten! " << colors["RESET"] << endl;
    cout << "Du kannst sie jederzeit in der settings.txt ändern" << endl;
    cout << "Bitte schreibe deinen System-Prompt in die prompt.txt" << endl << endl;

    ofstream("/tmp/say.txt") << textgreeting;
    system("espeak-ng -v de -f /tmp/say.txt");

    cout << "Welche Art von LLM benutzt du? (" << colors["YELLOW"] << "ollama" << colors["RESET"] << " für Ollama-Server, " << colors["YELLOW"] << "openai" << colors["RESET"] << " für Deepseek/ChatGPT/Claude und andere)" << endl;
    cin >> first_time_settings["llm_type"];

    if (first_time_settings["llm_type"] == "openai") {
        cout << endl << "Wo finde ich dein OpenAI? (zum Beispiel https://api.openai.com (OHNE /v1/chat/completions !))" << endl;
        cin >> first_time_settings["openai_address"];

        cout << endl << "Welches LLM-Modell benutzt du in OpenAI? (zum Beispiel gpt-4o, gpt-4o-mini, gpt-4, gpt-3.5-turbo)" << endl;
        cin >> first_time_settings["openai_model"];

        cout << endl << "Bitte gib deinen OpenAI API-Schlüssel ein (zum Beispiel sk-xxxxxxxxxxxxxxxxxxxxxx)" << endl;
        cin >> first_time_settings["llm_key"];
    }

    if (first_time_settings["llm_type"] == "ollama") {
        cout << endl << "Wo finde ich dein Ollama? (zum Beispiel 192.168.20.4:11434)" << endl;
        cin >> first_time_settings["ollama_address"];
        cout << endl << "Welches LLM-Modell benutzt du in Ollama? (zum Beispiel qwen2.5:7b)" << endl;
        cin >> first_time_settings["ollama_model"];
    }

    cout << endl << "Wo finde ich dein Whisper.cpp? (zum Beispiel http://192.168.20.4:10300/v1/audio/transcriptions)" << endl;
    cin >> first_time_settings["whisper_address"];

    cout << "Ist das dein erstes Mal, dass du BIT benutzt? (y/n)" << endl;
    string answer;
    cin >> answer;
    if (answer == "y" || answer == "Y")
        textgreeting2_enabled = true;
    else
        textgreeting2_enabled = false;

    cin.ignore(); // überspringt das verbleibende \n
    cout << endl << "Welche Begrüßung möchtest du für mich festlegen? (Ich werde sie bei jedem Start sagen)" << endl;
    getline(cin, first_time_settings["greeting"]);

    ofstream file("settings.txt");
    file << "*WILLKOMMEN IN DER BETA-VERSION DER SETTINGS-DATEI, BITTE BEFOLGE DIE ANWEISUNGEN UNTEN.\n";
    file << "*bitte verwende keine Leerzeichen als erstes oder letztes Zeichen der Zeile,\n";
    file << "*bitte ändere auch nicht die Werte nach den Rauten (#).\n";
    file << "*kommentare sind mit * am Anfang der Zeile markiert, du kannst dort alles hinzufügen, was du möchtest\n\n";


    file << "#piper_model_path_ru \n ./piper/ru_RU-ruslan-medium.onnx \n\n";
    file << "#piper_model_path_de \n ./piper/de_DE-thorsten-medium.onnx \n\n";
    file << "#piper_model_path_en \n ./piper/en_US-lessac-medium.onnx \n\n";
     
    
    for (auto &[key, value] : first_time_settings)
    {
        file << "#" << key << "\n"
             << value << "\n\n";
    }
    if(first_time_settings["language"]== "RU")
    file << "#piper_model \n " << first_time_settings["piper_model_path_ru"] << "\n\n";

    if(first_time_settings["language"]== "DE")
    file << "#piper_model \n " << first_time_settings["piper_model_path_de"] << "\n\n";

    if(first_time_settings["language"]== "EN")
    file << "#piper_model \n " << first_time_settings["piper_model_path_en"] << "\n\n";
   
    first_time = false;
}