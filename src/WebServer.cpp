#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>


// when the server receivees thee message from client
void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client

    */
   std::ifstream f("www/index.html"); // reead from file
   unsigned long size = std::filesystem::file_size("www/index.html");
   
   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Content-Length: "<< size <<"\r\n"
                           "\r\n"
                           << f.rdbuf();
    //oss<< f.rdbuf(); // copy buffer from filestream to stringstream
    

   sendToClient(client, oss.str().c_str(), oss.str().length());
   f.close();
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

