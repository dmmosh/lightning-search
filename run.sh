#!/bin/bash

git add .
git commit -am "nothing to note"
git push > /dev/null

g++ -std=c++20 \
    src/main.cpp src/WebServer.cpp src/TcpListener.cpp \
    -I/opt/libtorch/include \
    -I/opt/libtorch/include/torch/csrc/api/include \
    -ltorch -lcpr -lz \
    -o ls
