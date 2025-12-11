#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <iterator>
#include <fstream>
#include <vector>
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
   
   // parse out the document requested
   std::istringstream iss(msg);
   std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
   
   int errorCode = 404;
   std::ifstream f; // reead from file
   unsigned long size = 9;

    if(parsed.size() >= 3 && parsed[0] == "GET"){ // < request type > < file or endpoint > < http type >
        
        if(parsed[1] == ""){
            parsed[1] = "/index.html";
        }

        f.open("www"+parsed[1]);
        if(f.good()){
            errorCode = 200;
            size = std::filesystem::file_size("www"+parsed[1]);
        }
    }

   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 "<<errorCode<<" OK\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Cache-Control: public, max-age=31536000\r\n"
                            "ETag: \"lightning-search\"\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Content-Length: "<< size <<"\r\n"
                           "\r\n";
    if(f.good()){
        oss << f.rdbuf();
    } else {
        oss << "error 404";
    }
                           
    //oss<< f.rdbuf(); // copy buffer from filestream to stringstream
    

   sendToClient(client, oss.str().c_str(), oss.str().length());
   f.close();
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

