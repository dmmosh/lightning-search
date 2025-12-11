#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>



void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client

    */
   std::string out = "HTTP/1.1 200 OK\r\n"
                            "Cache-Control: no-cache, private\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Content-Length: 12\r\n"
                           "\r\n"
                           "Hello World!";
   sendToClient(client, out.c_str(), out.size()+1);
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

