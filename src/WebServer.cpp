#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


// when the server receivees thee message from client
void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client

    */
   std::ifstream f("../www/index.html", std::ios::binary | std::ios::ate); // reead from file
   unsigned long l = f.tellg();
   std::cout << l << '\n';
   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Transfer-Encoding: chunked\r\n"
                           "\r\n";
    oss<< f.rdbuf(); // copy buffer from filestream to stringstream

    

   sendToClient(client, oss.str().c_str(), 88+l);
   f.close();
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

