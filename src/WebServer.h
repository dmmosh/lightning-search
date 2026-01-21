#pragma once
#include "header.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <torch/torch.h>
#include <iterator>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <cstdlib> // Required for std::getenv
#include <cpr/cpr.h>
#include <regex>
#include <zlib.h>

#define MODEL_PATH "www/libtorch_ac_model.pt"
#define PORT 8080

// for multiple clients
extern const char* headers[]; 
extern torch::jit::script::Module model;
extern torch::Device device; // the device, all will go to it 

std::string compressGzip(const std::string& str); // compress gzip
unsigned int lastWord(const std::string& word); // client connected
std::vector<torch::jit::IValue> str_to_model(const std::string& input);

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