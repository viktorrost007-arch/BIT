#!/bin/bash
cd "$(dirname "$0")"

REPO="viktorrost007-arch/BIT"

echo "Узнаю последнюю версию..."
TAG=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name"' | head -1 | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$TAG" ]; then
    echo "не удалось узнать версию (нет релизов или нет сети)"
    exit 1
fi

echo "Последний релиз: $TAG"
echo "Качаю исходники..."

B="https://raw.githubusercontent.com/$REPO/$TAG"
curl -fsSL "$B/OllamaTest.cpp"          -o a.cpp  || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/OllamaTestFunctions.cpp" -o b.cpp  || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/OllamaTestHeaders.h"     -o c.h    || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/update.sh"     -o update.sh    || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/json.hpp"     -o json.hpp    || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/version.txt"     -o version.txt    || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }
curl -fsSL "$B/README.md"     -o README.md    || { echo "сбой"; rm -f a.cpp b.cpp c.h; exit 1; }

echo "Компилирую..."
g++ -std=c++17 -O2 -pthread a.cpp b.cpp -o ollamatest.new
if [ $? -ne 0 ]; then
    echo "ошибка сборки, старая версия работает"
    rm -f a.cpp b.cpp c.h ollamatest.new
    exit 1
fi

mv a.cpp OllamaTest.cpp
mv b.cpp OllamaTestFunctions.cpp
mv c.h   OllamaTestHeaders.h
mv ollamatest ollamatest.bak
mv ollamatest.new ollamatest
chmod +x ollamatest

echo "готово, версия $TAG"
echo "старая сохранена как ollamatest.bak"