#pragma once
#include "header.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <iterator>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <cstdlib> // Required for std::getenv
#include <regex>
#include <mutex>
#include <cpr/cpr.h>
#include <zlib.h>


// for multiple clients
extern const char* headers[]; 


class singleton{
    private:
        std::string name;

        static singleton* ptr;
        static std::mutex mtx;
        singleton(){}; 

    public:
        singleton(const singleton& obj) = delete;
        static singleton* getInstance();
        void set(const std::string& name);
        void print() const;
};

// for transition between backend and frontend
inline singleton* singleton ::ptr = nullptr; 
inline std::mutex singleton ::mtx;

std::string compressGzip(const std::string& str);
unsigned int lastWord(const std::string& word); // client connected

class WebServer : public TcpListener
{
public:
    WebServer(const char* ip, int port):
        TcpListener(ip, port) {};
    
protected:
cpr::AsyncResponse sendQuery(const char* query, unsigned int length);
virtual void onClientConnected(int client); // client connected
virtual void onClientDisconnected(int client); // client disconnected
virtual void onMessageReceived(int client, const char* msg, int length); // message is received from client
};