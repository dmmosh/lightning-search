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
   std::ostringstream oss;
   oss << "HTTP/1.1 200 OK\r\n\r\n"; 
   oss<< "Cache-Control: no-cache, private\r\n";
   oss<< "Content-Length: 5\r\n";
   oss << "\r\n";
   oss << "hello";

   sendToClient(client, oss.str().c_str(), oss.str().size()+1);
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

