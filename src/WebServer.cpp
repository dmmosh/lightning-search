#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <iterator>
#include <fstream>
#include <vector>
#include <filesystem>
#define QUERY parsed[1] // only use if search query specified 


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
   std::string query;
   unsigned long size = 9;
    for(int i=0; i<parsed.size(); i++){
        std::cout << parsed[i] << '\n';
    }


    // if a request has been made, 
   if(parsed.size() >= 3 && parsed[0] == "GET"){ // < request type > < file or endpoint > < http type >
    
    
        if(parsed[1].contains("/images")){ // if query contains images
            

        } else{ // if NOT images
            if(parsed[1] == "/"){ // reroute to home page 
                parsed[1] = "www/index.html";
            } else { // anything else is a search query , use QUERY macro
                query = parsed[1];
                parsed[1] = "www/search.html";
                
            }
            
            f.open(parsed[1]);
            if(f.good()){
                    errorCode = 200;
                    // size is the size of header, size of parsed file, and 
                    size = std::filesystem::file_size(parsed[1]);
            }
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

