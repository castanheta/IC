FROM ubuntu:20.04

RUN apt update && apt install -y \
    build-essential cmake git \
    libopencv-dev

WORKDIR /app

COPY . /app

RUN cmake . && make

ENTRYPOINT ["./image_processor"]
