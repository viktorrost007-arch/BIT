

# #Искренне извиняюсь но такого опыта написания скриптов как у нейронки у меня нет поэтому дал ей тезисы что должно быть а потом лапками правил и добавлял по подобию

#!/bin/bash
set -e

REPO="viktorrost007-arch/BIT"
echo "=== BIT AUTO-UPDATER ==="

# 1. Проверка базовых зависимостей (на случай, если их нет)
echo "[1/6] Checking system dependencies..."
if ! command -v curl &> /dev/null; then
    sudo apt-get update -y && sudo apt-get install -y curl
fi
if ! command -v g++ &> /dev/null; then
    sudo apt-get update -y && sudo apt-get install -y g++
fi

# 2. Получаем информацию о последнем релизе
echo "[2/6] Fetching latest release info..."
TAG=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name"' | head -1 | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$TAG" ]; then
    echo "Error: Could not find latest release on GitHub."
    exit 1
fi

echo "Latest release found: $TAG"

# 3. Скачиваем и распаковываем новый код
echo "[3/6] Downloading source code for $TAG..."
curl -fsSL "https://github.com/$REPO/archive/refs/tags/$TAG.tar.gz" -o repo_update.tar.gz
tar -xzf repo_update.tar.gz

FOLDER="BIT-${TAG#v}"

# 4. Обновляем файлы (НЕ трогаем settings.txt и prompt.txt)
echo "[4/6] Updating source files..."
cp "$FOLDER"/*.cpp .
cp "$FOLDER"/*.h .
cp "$FOLDER"/*.json .

# Если у пользователя каким-то чудом нет дефолтных настроек, кладем их
if [ ! -f "settings.txt" ] && [ -f "$FOLDER/settings.txt" ]; then cp "$FOLDER/settings.txt" .; fi
if [ ! -f "prompt.txt" ] && [ -f "$FOLDER/prompt.txt" ]; then cp "$FOLDER/prompt.txt" .; fi

# Убираем скачанный архив и распакованную папку
rm -rf "$FOLDER" repo_update.tar.gz

# 5. Проверяем Piper (если нет — качаем)
echo "[5/6] Checking Piper TTS and voice models..."
mkdir -p piper
if [ ! -f "piper/piper" ]; then
    echo "Piper not found. Downloading..."
    curl -fL -o piper.tar.gz https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz
    tar -xzf piper.tar.gz
    rm piper.tar.gz
    chmod +x piper/piper
fi

# Проверяем модели (скачиваем только те, которых нет)
if [ ! -f "piper/ru.onnx" ]; then
    curl -fL -o piper/ru.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx
    curl -fL -o piper/ru.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx.json
fi
if [ ! -f "piper/en.onnx" ]; then
    curl -fL -o piper/en.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
    curl -fL -o piper/en.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json
fi
if [ ! -f "piper/de.onnx" ]; then
    curl -fL -o piper/de.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx
    curl -fL -o piper/de.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/de/de_DE/thorsten/medium/de_DE-thorsten-medium.onnx.json
fi

# 6. Компиляция и замена бинарника
echo "[6/6] Compiling BIT..."
# Собираем все .cpp файлы в папке
g++ -std=c++17 -O2 -pthread *.cpp -o ollamatest.new

# Бэкап старого бинарника и замена
if [ -f "ollamatest" ]; then
    mv ollamatest ollamatest.bak
fi
mv ollamatest.new ollamatest
chmod +x ollamatest

echo "========================================="
echo "   UPDATE COMPLETED SUCCESSFULLY ($TAG)  "
echo "========================================="
echo "Your settings and prompt are safe."
echo "Old binary backed up as ollamatest.bak"

# Спрашиваем, запускать ли новую версию
read -p "Do you want to start the new version now? (y/n): " RUN_NOW
if [[ "$RUN_NOW" == "y" || "$RUN_NOW" == "Y" ]]; then
    ./ollamatest
fi