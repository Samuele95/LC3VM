FROM debian:sid AS build
LABEL Description="LC3 Virtual Machine"

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    make \
    flex \
    bison \
    libfl-dev \
    libbison-dev \    
    libpthread-stubs0-dev \
    gdb 

RUN mkdir lc3vm
WORKDIR /lc3vm

COPY src/ ./src/
COPY resources/ ./resources/
COPY lc3install.sh .
COPY Makefile .

RUN bash ./lc3install.sh install
