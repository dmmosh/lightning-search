#include "WebServer.h"


/*

"Cache-Control: public, max-age=31536000\r\n"
*/

const char* headers[] = {
    "Connection: Keep-Alive\r\n"  
    "ETag: \"lightning-search-html\"\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n",
    
    "Content-Type: image/x-icon\r\n"
    "Connection: close\r\n",

    "Content-Type: text/css\r\n",
    "ETag: \"lightning-search-css\"\r\n",

    "Content-Type: text/javascript\r\n"
    "ETag: \"lightning-search-js\"\r\n"

    "Content-Type: text/plain\r\n"
    "ETag: \"lightning-search-plain\"\r\n"
}; 



// macros for headers
#define H_PAGE 0  // header for pages
#define H_IMAGE 1 // header for images
#define H_CSS 2 // header for css
#define H_JS 3 // header for js
#define H_ERROR 4 // headder for invalid 
#define SIZE_ERROR 9 // size of error 404 page (9 chars)

#define QUERY parsed // only use if search query specified 

using json = nlohmann::json;
int h_num = H_ERROR; // header number
char* env_key = "EXA1"; // goes up (env_key[3]) until it cant anymore 
char* key = std::getenv(env_key);



cpr::AsyncResponse WebServer::sendQuery(const char* query, unsigned int length){
    
    json body = {
        {"query",query},
        {"type", "fast"}
    };
    

    // regex pattern for website matching 
    // if website is found, itll exclusively search 
    const std::regex pattern("(?:^|\\s|\\+|%20)(([a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)+)(?![:/.\\w]))");
    std::cmatch matches;
    // Use cregex_iterator for const char arrays
    std::cregex_iterator it(query, query+length, pattern);
    std::cregex_iterator end; // Default constructor creates an end-of-sequence iterator

    //std::cout << "Found matches:" << std::endl;
    while (it != end) {
        std::cmatch match = *it;
        body["includeDomains"].push_back(match[1].str());
        //std::cout << "*\t" << match[1].str()<< '\n';
        ++it;
    }
    //std::cout << body << '\n';


    //return cpr::PostAsync(cpr::Url{"https://httpbin.org/post"});
    return cpr::PostAsync(
                        cpr::Url{"https://api.exa.ai/search"},

                        cpr::Header{{"Content-Type","application/json"},
                                    {"x-api-key", (const char*)key}},
                        cpr::Body{body.dump().c_str()}
                                );
                                
                                // cpr::Body{std::format("{\"query\": \"{}\",")}
                                //             "\"type\": \"fast\"}",
                                //             }

};


// when the server receivees thee message from client
void WebServer::onMessageReceived(int client, const char* msg, int length){

    /*
    client (google) will sent a message in the form of:
    GET /index.html HTTP/1.1

    we have to parse out the document requested
    open the document in the local file system
    write the document back to the client


    when a file imports another file , client calls a message to the given 

    only thing parsed: GET <header> , dont need other info as its organized into folders
    */
   
   // parse out the document requested
   //std::istringstream iss(msg);

  

   //std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

   std::string parsed; 
    if(!strncmp(msg,"GET",3)){
        unsigned int i = 4; // starts at 2nd argument 
        while(i<length and msg[i] != ' '){ // goes until end of string or blank char
            i++;
        }
        parsed = std::string(msg+4,i-4);
    }

   int errorCode = 404;
   std::ifstream f; // reead from file
   //std::cout << key << '\n';
   unsigned long size = SIZE_ERROR;
//    std::cout << "{\n";
//     std::cout << msg;
//    std::cout << "}\n";
    cpr::AsyncResponse resp;
   const char* url = parsed.c_str()+1; // the url without the / at beginning, use strcmp and strncmp


    // if a request has been made, 
   if(!parsed.empty() && parsed[0] == '/'){ // < request type > < file or endpoint > < http type >

        if(url[0] == '?'){ // search query , has to be preceded by / because files cant be named as such 
            resp = sendQuery(url+1, parsed.length()-2); // everything after / and question mark is searched
            h_num = H_PAGE;
            parsed = "/search.html"; //
        }else if(url[0] == '\0'){ // if nothing , main page
            h_num = H_PAGE;
            parsed = "/index.html";
        } else if(!strncmp(url, "css", 4)){ // if the folder is css
            h_num = H_CSS;
        } else if(!strncmp(url, "images", 7)){ // images are being loaded
            h_num = H_IMAGE;
            parsed.insert(0,"/images");
        } else if(!strncmp(url, "js", 2)){ // js code is being loaded
            h_num = H_JS;
        }
        
        
        parsed.insert(0,"www");
        if(h_num == H_IMAGE){
            f.open(parsed, std::ios::in | std::ios::binary); // open image in binary mode  
        } else if (h_num != H_ERROR){
            f.open(parsed);
        }
        
        //std::cout << parsed << '\n';
        if(f.is_open() && f.good()){
            errorCode = 200;
            // size is the size of header, size of parsed file, and 
            size = std::filesystem::file_size(parsed);
        }
        //std::cout << h_num << '\t' << parsed<< '\n';
        
    }
    

    std::string results = "<script>let results =;</script>";
    if(resp.valid()){ // a search is going on!!
        cpr::Response out = resp.get(); // wait for response

        if(out.status_code == 200){ // everything worked out
            results.insert(21,out.text);    
        } else { // if the api request fails, display error msg
            errorCode = 404;
            h_num = H_ERROR;
            size = SIZE_ERROR;
        }
    }
    
    std::cout << results << '\n';
   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 "<<errorCode<<" OK\r\n"
                            << headers[h_num] << 
                           "Content-Length: "<< size <<"\r\n"
                           "\r\n";
    
    if(f.is_open() && f.good()){ // write the opened file's buffer into ostringstream
        oss << f.rdbuf();
        f.close(); // close the file
    } else {
        oss << "error 404";
    }
                           
    //oss<< f.rdbuf(); // copy buffer from filestream to stringstream
    

   sendToClient(client, oss.str().c_str(), oss.str().length()); // send to client
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

