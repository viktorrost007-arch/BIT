#!/usr/bin/env bash
rm -rf ~/BIT-project
mkdir -p ~/BIT-project
cd ~/BIT-project
echo "вошел в папку проекта"
rm -rf BIT
git clone https://github.com/viktorrost007-arch/BIT.git
cd BIT
echo "\n Компилирую (молись)"
g++ OllamaTest.cpp OllamaTestFunctions.cpp -lpthread -o ollamatest

read -p " Готово, запустить эту версию? (y/n) \n: " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Запуск отменен."
    exit 0
fi
clear
echo "Приятного пользования! \n"
./ollamatest
exit 0;