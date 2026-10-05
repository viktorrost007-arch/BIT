#include "OllamaTestHeaders.h"





int main() {


  history.push_back({ {"role","system"}, {"content", sysprompt} });
  cout<< greeting << endl;
  std::string command = "espeak-ng -v en \"" + textgreeting+ "\"";
  std::system(command.c_str());

  cout<< "Wah do you wanna ask?";

  while (getline(cin, uinput)) {

    cout << uinput<< endl;

   for(int i = 0; i < 15; i++){

   if(uinput == "/exit") break;

    if (uinput.empty()) {
      if (!recording) {
        startRec();
        recording = true;
        cout << "rec...\n";
        continue;                      // ← выходим, ничего больше не делаем
      } else {
        stopRec();
        recording = false;

        string uinput = SendToWhisper();
        cout << "you> " << uinput  << "\n";
        //if (text.empty()) continue;

        history.push_back({ {"role","user"}, {"content", uinput} });
        SendToOllama();
      }
    } else {
      history.push_back({ {"role","user"}, {"content", uinput} });
      SendToOllama();
    }


   } //for 1



    cout << "bit> \"" <<uianswer << """" << "\"\n";
    string speechcommand = "espeak-ng -v ru \"" + uianswer + "\"";
    system(speechcommand.c_str());

  }
  remove("test.wav");
  return 0;
}
