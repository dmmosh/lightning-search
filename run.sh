#!/bin/bash

git add .
git commit -am "nothing to note"
git push > /dev/null

g++ -std=c++20 \
    -I/opt/libtorch/include \
    -I/opt/libtorch/include/torch/csrc/api/include \
    -L/opt/libtorch/lib \
    -ltorch \ 
    -lcpr \
    src/main.cpp src/WebServer.cpp src/TcpListener.cpp \
    -o ls
