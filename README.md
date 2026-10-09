# BIT
## Зависимости

- g++ (C++17)
- libc6-dev
- alsa-utils (arecord)
- espeak-ng
- curl
- httplib.h (header-only, в репозитории)
- nlohmann/json (header-only, в репозитории)


#!/bin/bash
sudo apt install g++ libc6-dev alsa-utils espeak-ng curl
g++ -std=c++17 -O2 -pthread OllamaTest.cpp OllamaTestFunctions.cpp -o ollamatest


## Внешние сервисы (должны быть запущены отдельно)

- Ollama на `host:11434` с моделью qwen2.5:7b
- Whisper-совместимый сервер на `host:10300` (endpoint /v1/audio/transcriptions)
