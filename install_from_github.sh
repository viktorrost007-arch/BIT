#!/bin/bash
set -e

REPO="viktorrost007-arch/BIT"
echo "=== BIT INSTALLER (LATEST RELEASE) ==="

# 1. Установка зависимостей
echo "[1/6] Installing system dependencies..."
sudo apt-get update -y
sudo apt-get install -y g++ curl alsa-utils espeak-ng

# 2. Узнаем последний релиз и качаем его
echo "[2/6] Getting latest release version..."
TAG=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name"' | head -1 | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$TAG" ]; then
    echo "Error: Could not find latest release."
    exit 1
fi

echo "Latest release: $TAG"
curl -fsSL "https://github.com/$REPO/archive/refs/tags/$TAG.tar.gz" -o repo.tar.gz
tar -xzf repo.tar.gz

FOLDER="BIT-${TAG#v}"
cp "$FOLDER"/*.cpp .
cp "$FOLDER"/*.h .
cp "$FOLDER"/*.json .
if [ ! -f "settings.txt" ] && [ -f "$FOLDER/settings.txt" ]; then cp "$FOLDER/settings.txt" .; fi
if [ ! -f "prompt.txt" ] && [ -f "$FOLDER/prompt.txt" ]; then cp "$FOLDER/prompt.txt" .; fi
rm -rf "$FOLDER" repo.tar.gz

# 3. Библиотеки C++
echo "[3/6] Downloading C++ libraries..."
if [ ! -f "json.hpp" ]; then curl -fsSL -o json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp; fi
if [ ! -f "httplib.h" ]; then curl -fsSL -o httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h; fi

# 4. Piper TTS
echo "[4/6] Setting up Piper TTS..."
mkdir -p piper
if [ ! -f "piper/piper" ]; then
    curl -fL -o piper.tar.gz https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz
    tar -xzf piper.tar.gz
    rm piper.tar.gz
    chmod +x piper/piper
fi

# 5. Голосовые модели (RU, EN, DE)
echo "[5/6] Downloading voice models..."
# Русский (Руслан)
if [ ! -f "piper/ru.onnx" ]; then
    curl -fL -o piper/ru.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx
    curl -fL -o piper/ru.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx.json
fi
# Английский (Lessac)
if [ ! -f "piper/en.onnx" ]; then
    curl -fL -o piper/en.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
    curl -fL -o piper/en.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
fi
# Немецкий (Thorsten)
if [ ! -f "piper/de.onnx" ]; then
    curl -fL -o piper/de.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx
    curl -fL -o piper/de.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx.json
fi

# 6. Компиляция
echo "[6/6] Compiling BIT..."
g++ -std=c++17 -O2 -pthread *.cpp -o ollamatest
chmod +x ollamatest

echo "=== INSTALLATION COMPLETE ($TAG) ==="
echo "Run: ./ollamatest"