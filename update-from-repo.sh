#!/usr/bin/env bash
#Единственный и первый самописный скрипт (шучу)....
read -p $'\033[32m Ты уверен что стоит, ведь это не поставит бинарник и может поломать все, а еще и удалит папку проекта! Ну как? Страшно? (y/n) \033[0m\n:' CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Запуск отменен."
    exit 0
fi

rm -rf ~/BIT-project
mkdir -p ~/BIT-project
cd ~/BIT-project
echo "вошел в папку проекта"
rm -rf BIT
git clone https://github.com/viktorrost007-arch/BIT.git
cd BIT
echo "\n Компилирую (молись)"
g++ OllamaTest.cpp OllamaTestFunctions.cpp -lpthread -o bit

read -p " Готово, запустить эту версию? (y/n) \n: " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Запуск отменен."
    exit 0
fi
clear
echo "Приятного пользования! \n"
./bit
exit 0;