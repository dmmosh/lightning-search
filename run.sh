#!/bin/bash

git add .
git commit -am "nothing to note"
git push

g++ -std=c++20 src/main.cpp src/WebServer.cpp src/TcpListener.cpp -o ls
