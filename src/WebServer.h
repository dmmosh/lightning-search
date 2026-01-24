#pragma once
#include "header.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <torch/torch.h>
#include <stdexcept> // Contains standard exception types
#include <iterator>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <cstdlib> // Required for std::getenv
#include <cpr/cpr.h>
#include <regex>
#include <zlib.h>
#include <optional>

#define MODEL_PATH "www/libtorch_ac_model.pt"
#define PORT 8080

// the required variables to run the autocorrect model
typedef struct model_vars_struct{
    torch::Dict<int64_t,std::string> iword;
    int block_size;
    char stop_word;

    
} model_vars_struct;

extern model_vars_struct model_vars;
// for multiple clients
extern const char* headers[]; 
extern torch::jit::script::Module module;
extern torch::Device device; // the device, all will go to it 


// compress gzip
std::string compressGzip(const std::string& str); // compress gzip

// get the last word
unsigned int lastWord(const std::string& word); // client connected

// string to model (vector of ivalues)
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