FROM gcc:latest AS builder
WORKDIR /app
COPY *.cpp *.h json.hpp httplib.h ./
RUN g++  -lpthread *.cpp -o bit-server

FROM debian:stable-slim
WORKDIR /app
RUN apt-get update && apt-get install -y curl libespeak-ng1 && rm -rf /var/lib/apt/lists/*

# Скачиваем Piper и модели ПРЯМО В ОБРАЗ при сборке
RUN mkdir -p /app/piper && cd /app/piper && \
    curl -L -o piper.tar.gz https://github.com/rhasspy/piper/releases/latest/download/piper_linux_x86_64.tar.gz && \
    tar -xzf piper.tar.gz && \
    rm piper.tar.gz && \
    chmod +x piper && \
    curl -L -o ru.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx && \
    curl -L -o ru.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/ru/ru_RU/ruslan/medium/ru_RU-ruslan-medium.onnx.json && \
    curl -L -o en.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx && \
    curl -L -o en.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json

COPY --from=builder /app/bit-server ./
COPY tools.json ./
CMD ["./bit-server"]