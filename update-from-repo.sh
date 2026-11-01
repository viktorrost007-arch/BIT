#!/bin/bash
set -e

REPO="viktorrost007-arch/BIT"
echo "=== BIT UPDATER (MAIN COMMIT) ==="

# 1. Зависимости и WSL-фикс
echo "[1/6] Installing system dependencies..."
sudo apt-get update -y
sudo apt-get install -y g++ curl alsa-utils espeak-ng

if grep -qi microsoft /proc/version; then
    echo "WSL detected. Installing PulseAudio..."
    sudo apt-get install -y pulseaudio pulseaudio-utils libasound2-plugins
    echo "pcm.!default { type pulse } ctl.!default { type pulse }" > ~/.asoundrc
fi

# 2. Скачивание исходников из main
echo "[2/6] Downloading latest source from main..."
curl -fsSL "https://github.com/$REPO/archive/refs/heads/main.tar.gz" -o repo.tar.gz
tar -xzf repo.tar.gz

cp BIT-main/*.cpp .
cp BIT-main/*.h .
cp BIT-main/*.json .
if [ ! -f "settings.txt" ] && [ -f "BIT-main/settings.txt" ]; then cp BIT-main/settings.txt .; fi
if [ ! -f "prompt.txt" ] && [ -f "BIT-main/prompt.txt" ]; then cp BIT-main/prompt.txt .; fi
rm -rf BIT-main repo.tar.gz

# 3. Библиотеки C++
echo "[3/6] Checking C++ libraries..."
if [ ! -f "json.hpp" ]; then curl -fsSL -o json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp; fi
if [ ! -f "httplib.h" ]; then curl -fsSL -o httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h; fi

# 4. Piper TTS
echo "[4/6] Checking Piper TTS..."
mkdir -p piper
if [ ! -f "piper/piper" ]; then
    curl -fL -o piper.tar.gz https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz
    tar -xzf piper.tar.gz
    rm piper.tar.gz
    chmod +x piper/piper
fi

# 5. Голосовые модели (RU, EN, DE)
echo "[5/6] Checking voice models..."
# Русский
if [ ! -f "piper/ru.onnx" ]; then
    curl -fL -o piper/ru.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx
    curl -fL -o piper/ru.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx.json
fi
# Английский
if [ ! -f "piper/en.onnx" ]; then
    curl -fL -o piper/en.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
    curl -fL -o piper/en.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
fi
# Немецкий
if [ ! -f "piper/de.onnx" ]; then
    curl -fL -o piper/de.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx
    curl -fL -o piper/de.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx.json
fi

# 6. Компиляция
echo "[6/6] Compiling BIT..."
g++ -std=c++17 -O2 -pthread *.cpp -o ollamatest
chmod +x ollamatest

echo "=== UPDATE COMPLETE ==="