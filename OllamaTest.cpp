#include "OllamaTestHeaders.h"

//VERSION 1.0.0



int main() {
  system("which arecord espeak-ng curl >/dev/null 2>&1 || echo 'ВНИМАНИЕ: не все зависимости установлены'");
  cout<< greeting << endl; //BIT

  checkfirst_time();
  system("clear");
  cout << greeting << endl;
  cout << greeting2 << endl;
  readSettings();
  //cout << greeting2 << endl; 
  
  textgreeting = settings["greeting"];
  

  history.push_back({ {"role","system"}, {"content", sysprompt} });
  ofstream("/tmp/say.txt") << textgreeting;
    system("espeak-ng -v en -f /tmp/say.txt");
  // std::string command = "espeak-ng -v en \"" + textgreeting+ "\"";
  // std::system(command.c_str());

  cout<< "What do you wanna ask?"<< endl;

  handleInput();
exit();
  
  
  return 0;
}
// еще будет выбор языка из настроек чтобы избежать странного акцента при espeak c en