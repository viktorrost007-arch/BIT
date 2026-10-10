#include "OllamaTestHeaders.h"





int main() {

  readSettings();
  textgreeting = settings["greeting"];

  history.push_back({ {"role","system"}, {"content", sysprompt} });
  cout<< greeting << endl;
  ofstream("/tmp/say.txt") << textgreeting;
    system("espeak-ng -v en -f /tmp/say.txt");
  // std::string command = "espeak-ng -v en \"" + textgreeting+ "\"";
  // std::system(command.c_str());

  cout<< "What do you wanna ask?"<< endl;

  while (getline(cin, uinput)) {

  //cout << uinput<< endl;
 
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
        if (uinput.empty()) continue;

        history.push_back({ {"role","user"}, {"content", uinput} });
        while(SendToOllama()){}
      }
    } else {
      history.push_back({ {"role","user"}, {"content", uinput} });
     while( SendToOllama()){}
    }


  



    //cout << "BIT> \"" <<uianswer << """" << "\"\n";
    cout << "BIT>  " << uianswer << "    " << "\n";
    // string speechcommand = "espeak-ng -v ru \"" + uianswer + "\"";
    // system(speechcommand.c_str());
    ofstream("/tmp/say.txt") << uianswer;
    system("espeak-ng -v ru -f /tmp/say.txt");

  }
  remove("test.wav");
  return 0;
}
