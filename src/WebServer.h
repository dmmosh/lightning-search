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
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

// for multiple clients
extern const char* headers[]; 
extern const char* category[]; 


class WebServer : public TcpListener
{
public:
    WebServer(const char* ip, int port):
        TcpListener(ip, port) {};
    
protected:
cpr::AsyncResponse sendQuery(const char* query, unsigned int length);
unsigned int lastWord(const std::string& word); // client connected
virtual void onClientConnected(int client); // client connected
virtual void onClientDisconnected(int client); // client disconnected
virtual void onMessageReceived(int client, const char* msg, int length); // message is received from client
};