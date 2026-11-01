#!/bin/bash
set -e

echo "=== BIT INSTALLER ==="

# 1. Скачиваем исходники
echo "[1/4] Downloading source code..."
curl -fsSL https://github.com/viktorrost007-arch/BIT/archive/refs/heads/main.tar.gz -o repo.tar.gz
tar -xzf repo.tar.gz

# Создаем папку BIT и переходим в нее
mkdir -p BIT
cd BIT

# Копируем наши файлы из распакованного архива
cp ../BIT-main/*.cpp .
cp ../BIT-main/*.h .
cp ../BIT-main/*.json .

# 2. Скачиваем header-only библиотеки (если их нет в репо)
echo "[2/4] Downloading dependencies..."
if [ ! -f "json.hpp" ]; then
    curl -fsSL -o json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
fi
if [ ! -f "httplib.h" ]; then
    curl -fsSL -o httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
fi

# 3. Скачиваем Piper (пропускаем, если уже есть)
echo "[3/4] Setting up Piper..."
mkdir -p piper
cd piper
if [ ! -f "piper" ]; then
    curl -fL -o piper.tar.gz https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz
    tar -xzf piper.tar.gz
    mv piper/* .
    rm -rf piper piper.tar.gz
fi
if [ ! -f "ru.onnx" ]; then
    curl -fL -o ru.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx
    curl -fL -o ru.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx.json
fi
if [ ! -f "en.onnx" ]; then
    curl -fL -o en.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
    curl -fL -o en.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
fi
cd ..

# 4. Компиляция
echo "[4/4] Compiling..."
g++ -std=c++17 -O2 -pthread *.cpp -o ollamatest
chmod +x ollamatest

# Уборка
cd ..
rm -rf BIT-main repo.tar.gz

echo "=== INSTALLATION SUCCESSFUL ==="
echo "Your project is in ~/BIT"
echo "Run: cd ~/BIT && ./ollamatest"