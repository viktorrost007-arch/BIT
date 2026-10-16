#include "OllamaTestHeaders.h"





int main() {
  system("which arecord espeak-ng curl >/dev/null 2>&1 || echo 'ВНИМАНИЕ: не все зависимости установлены'");
  cout<< greeting << endl; //BIT

  checkfirst_time();
  readSettings();

  
  textgreeting = settings["greeting"];
  system("clear");

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
   if( uinput == "/setup") setup();
   if( uinput == "/reset") reset();

    if (uinput.empty()) {
      if (!recording) {
        startRec();
        recording = true;
        cout << "rec...\n";
        continue;                      // ← выходим, ничего больше не делаем
      } else {
        stopRec();
        cout << "\r               " << flush << endl << endl;
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
    cout << "BIT>  " << uianswer << " " << endl;
    // string speechcommand = "espeak-ng -v ru \"" + uianswer + "\"";
    // system(speechcommand.c_str());
    ofstream("/tmp/say.txt") << uianswer;
    system("espeak-ng -v ru -f /tmp/say.txt");

  }
   cout << "Exiting.";
  remove("test.wav");
  cout << ".";
  remove("/tmp/say.txt");
  cout << "."<< endl; 
  cout << "Goodbye!"<< endl;
  system("espeak-ng -v en \"goodbye!\"");

  system("clear");

  
  
  return 0;
}
