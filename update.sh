#!/bin/bash

# Приветствие
read -p "Вы уверены, что хотите обновить BIT? (y/n): " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Обновление отменено."
    exit 0
fi

cd "$(dirname "$0")"
REPO="viktorrost007-arch/BIT"

echo "Узнаю последнюю версию..."
TAG=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name"' | head -1 | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$TAG" ]; then
    echo "Не удалось узнать версию (нет релизов или нет сети)."
    exit 1
fi

echo "Последний релиз: $TAG"
echo "Качаю исходники..."

B="https://raw.githubusercontent.com/$REPO/$TAG"

# Скачиваем всё, включая библиотеки и tools.json
curl -fsSL "$B/OllamaTest.cpp"          -o a.cpp          || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/OllamaTestFunctions.cpp" -o b.cpp          || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/OllamaTestHeaders.h"     -o c.h            || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/json.hpp"                -o json.hpp       || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/httplib.h"               -o httplib.h      || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/tools.json"              -o tools.json     || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/version.txt"             -o version.txt    || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }
curl -fsSL "$B/README.md"               -o README.md      || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }

# Новый скрипт качаем отдельно, чтобы не сломать текущий, пока он выполняется
curl -fsSL "$B/update.sh"               -o update.sh.new  || { echo "сбой"; rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json; exit 1; }

echo "Компилирую..."
g++ -std=c++17 -O2 -pthread a.cpp b.cpp -o ollamatest.new
if [ $? -ne 0 ]; then
    echo "ОШИБКА СБОРКИ! Старая версия оставлена рабочей."
    rm -f a.cpp b.cpp c.h json.hpp httplib.h update.sh.new tools.json ollamatest.new
    exit 1
fi

echo "Обновляю файлы..."
mv a.cpp OllamaTest.cpp
mv b.cpp OllamaTestFunctions.cpp
mv c.h   OllamaTestHeaders.h
mv update.sh.new update.sh
chmod +x update.sh

# Бэкап старого бинарника, только если он существует
if [ -f ollamatest ]; then
    mv ollamatest ollamatest.bak
fi
mv ollamatest.new ollamatest
chmod +x ollamatest

# Используем $'\033[32m' для зеленого цвета в bash
echo -e "\033[32mГотово, версия $TAG\033[0m"
echo "Старая версия сохранена как ollamatest.bak"
echo "Не забывайте заходить на GitHub: в новых версиях могут быть новые файлы."

read -p $'\033[32mЗапустить новую версию BIT? (y/n): \033[0m' CONFIRM
if [[ "$CONFIRM" == "y" || "$CONFIRM" == "Y" ]]; then
    echo "Приятного пользования новой версией! При ошибках скачайте версию с GitHub."
    clear
    ./ollamatest
else
    echo "Вы можете запустить новую версию позже, введя ./ollamatest"
    exit 0
fi