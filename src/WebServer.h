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
#include <cpr/cpr.h>

// for multiple clients
extern const char* headers[]; 

extern char* env_key;
extern char* key;
extern int h_num; // header num

class WebServer : public TcpListener
{
public:
    WebServer(const char* ip, int port):
        TcpListener(ip, port) {};
    
protected:
virtual void onClientConnected(int client); // client connected
virtual void onClientDisconnected(int client); // client disconnected
virtual void onMessageReceived(int client, const char* msg, int length); // message is received from client
};