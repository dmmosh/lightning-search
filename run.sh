#!/bin/bash

git add .
git commit -am "nothing to note"
git push > /dev/null

g++ -std=c++20 -lz -lcpr src/main.cpp src/WebServer.cpp src/TcpListener.cpp -o ls
