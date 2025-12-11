#include "WebServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <zlib.h>

// when the server receivees thee message from client
void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client

    */
   std::ifstream f("www/index.html", std::ios::binary); // read from file in binary mode
   std::ostringstream fileBuffer;
   fileBuffer << f.rdbuf();
   std::string fileContent = fileBuffer.str();

   // Compress the file content using gzip
   uLongf compressedSize = compressBound(fileContent.size());
   std::vector<char> compressedData(compressedSize);

   if (compress(reinterpret_cast<Bytef*>(compressedData.data()), &compressedSize, reinterpret_cast<const Bytef*>(fileContent.data()), fileContent.size()) != Z_OK) {
       std::cerr << "Failed to compress file content" << std::endl;
       return;
   }

   compressedData.resize(compressedSize);

   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 200 OK\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Cache-Control: public, max-age=31536000\r\n"
                            "Content-Encoding: gzip\r\n"
                            "ETag: \"lightning-search\"\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Content-Length: " << compressedData.size() << "\r\n"
                           "\r\n";

   sendToClient(client, oss.str().c_str(), oss.str().length());
   sendToClient(client, compressedData.data(), compressedData.size());
   f.close();
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


};

