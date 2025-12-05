#!/bin/bash

git add .
git commit -am "nothing to note"
git push

g++ src/main.cpp src/TcpListener.cpp -o ls
