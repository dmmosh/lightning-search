#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <iterator>
#include <fstream>
#include <vector>
#include <filesystem>

/*

"Cache-Control: public, max-age=31536000\r\n"
"Connection: close\r\n",
*/

const char* headers[] = {
    "Connection: Keep-Alive\r\n"  
    "ETag: \"lightning-search\"\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n",

    "Content-Type: image/x-icon\r\n"
    "Cache-Control: public, max-age=31536000\r\n"
    
}; 
int h_num = 0; // header number
// macros for headers
#define H_PAGE 0  // header for pages
#define H_IMAGE 1 // header for images

#define QUERY parsed[1] // only use if search query specified 



// when the server receivees thee message from client
void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client


    when a file imports another file , client calls a message to the given 
    */
   
   // parse out the document requested
   std::istringstream iss(msg);

  

   std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
   



   int errorCode = 404;
   std::ifstream f; // reead from file
   std::string query;
   unsigned long size = 9;
   std::cout << "{\n";
    for(const std::string& s: parsed){
        std::cout << s << '\t';
    }
   std::cout << "}\n";



    // if a request has been made, 
   if(parsed.size() >= 3 && parsed[0] == "GET"){ // < request type > < file or endpoint > < http type >
    
    
        if(parsed[1].find("/images") != std::string::npos){ // if query contains images
            h_num = H_IMAGE;
            f.open(parsed[1]);
            if(f.good()){
                errorCode = 200;
                // size is the size of header, size of parsed file, and 
                size = std::filesystem::file_size(parsed[1]);
            }

        } else{ // if NOT images (pages)
            h_num = H_PAGE;

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
                            << headers[h_num] << 
                           "Content-Length: "<< size <<"\r\n"
                           "\r\n";
    
    if(f.good()){ // write the opened file's buffer into ostringstream
        oss << f.rdbuf();
    } else {
        oss << "error 404";
    }
                           
    //oss<< f.rdbuf(); // copy buffer from filestream to stringstream
    

   sendToClient(client, oss.str().c_str(), oss.str().length()); // send to client
   f.close(); // close the file
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

