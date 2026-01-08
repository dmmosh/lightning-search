#!/bin/bash

git add .
git commit -am "nothing to note"
git push > /dev/null

g++ -std=c++20 -DCMAKE_USE_COLVARS=none -lz -lcpr src/main.cpp src/WebServer.cpp src/TcpListener.cpp -o ls
