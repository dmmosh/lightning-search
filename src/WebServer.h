#pragma once
#include "header.h"

// for multiple clients
extern const char* headers[]; 
extern int h_num; // header num

class WebServer : public TcpListener
{
public:
    WebServer(const char* ip, int port):
        TcpListener(ip, port) {};
    
protected:
virtual void onClientConnected(int client); // client connected
virtual void onClientDisconnected(int client); // client disconnected
virtual void onMessageReceived(int client, char* msg, int length); // message is received from client
};