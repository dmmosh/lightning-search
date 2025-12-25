#include "WebServer.h"


/*

"Cache-Control: public, max-age=31536000\r\n"
*/

const char* headers[] = {
    "text/html",
    "image/x-icon",
    "text/css",
    "text/javascript",
    "text/plain",
    "application/json"
}; 




// macros for headers
#define H_PAGE 0  // header for pages
#define H_IMAGE 1 // header for images
#define H_CSS 2 // header for css
#define H_JS 3 // header for js
#define H_PLAIN 4 // headder for invalid 
#define H_JSON 5
#define SIZE_ERROR 9 // size of error 404 page (9 chars)

#define QUERY parsed // only use if search query specified 

//using json = nlohmann::json;
int h_num = H_PLAIN; // header number
const char* env_key = (const char*)"EXA1"; // goes up (env_key[3]) until it cant anymore 
char* key = std::getenv(env_key);



cpr::AsyncResponse WebServer::sendQuery(const char* query, unsigned int length){
    
    // json body = {
    //     {"query",query},
    //     {"type", "fast"}
    // };
    
    

    // regex pattern for website matching 
    // if website is found, itll exclusively search 
    static const std::regex pattern("(?:^|\\s|\\+|%20)(([a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)+)(?![:/.\\w]))");
    //std::cmatch matches;
    // Use cregex_iterator for const char arrays
    std::cregex_iterator it(query, query+length, pattern);
    std::cregex_iterator end; // Default constructor creates an end-of-sequence iterator

    std::ostringstream oss; // output stream
    oss << "{ \"type\": \"fast\", "
                 "\"query\": \"" << query << '\"';

    //std::cout << "Found matches:" << std::endl;
    if(it != end){ // if matches found, (iterator is not at the end)
        oss << ", \"includeDomains\": [";

        while (it != end) {
            std::cmatch match = *it;
            //body["includeDomains"].push_back(match[1].str());
            oss << '\"' << match[1].str() << '\"';
            //std::cout << "*\t" << match[1].str()<< '\n';
            ++it;
            if(it !=end){ // if currently is NOT the last element,
                oss << ',';
            }
        }    
        oss << ']';

    }
    
    
    oss << '}';
    //std::cout << body << '\n';


    //return cpr::PostAsync(cpr::Url{"https://httpbin.org/post"});
    return cpr::PostAsync(
                        cpr::Url{"https://api.exa.ai/search"},

                        cpr::Header{{"Content-Type","application/json"},
                                    {"x-api-key", (const char*)key}},
                        cpr::Body{oss.str().c_str()}
                                );
                                
                                // cpr::Body{std::format("{\"query\": \"{}\",")}
                                //             "\"type\": \"fast\"}",
                                //             }

};

unsigned int WebServer::lastWord(const std::string& word){
    // word is guranteed to start at ac?
    // will iterate from the end to the beginning
    unsigned int i = word.length(); // will always check the character before and return the current one
    while(i>0){
        switch(word[i-1]){
            case '?': // end of word
            case '+': // space 
                return i;
                break;
            case '0':
                if(word[i-2] == '2' && word[i-3] == '%') { // if the previous 3 characters are %20,
                    return i;
                }
                break;
            default: // every other char 
                i-=1;
        }
    }
    return word.length();

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
        } else if(!strncmp(url, "ac?", 3)){ // autocomplete feature as defined by opensearch.xml file
            parsed = std::string("[\"")+ (parsed.c_str()+3) + "\": [\""+ (parsed.c_str()+3) +" \"]]";
            std::coud << parsed << '\n';
            errorCode = 200;
            size = parsed.length()-1;
            h_num=H_JSON;
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
        
        
        if(h_num == H_IMAGE){
            parsed.insert(0,"www");
            f.open(parsed, std::ios::in | std::ios::binary); // open image in binary mode  
        } else if (h_num != H_PLAIN){
            parsed.insert(0,"www");
            f.open(parsed);
        }
        
        //std::cout << parsed << '\n';
        // if file-opening requests are good, record size and error code
        if(f.is_open() && f.good()){
            errorCode = 200;
            // size is the size of header, size of parsed file, and 
            size = std::filesystem::file_size(parsed);
        }
        //std::cout << h_num << '\t' << parsed<< '\n';
        
    }
    

    std::string results = "<script>let results =;</script>";
    cpr::Response out;
    if(resp.valid()){ // a search is going on!!
        out = resp.get(); // wait for response

        if(out.status_code == 200){ // everything worked out
            results.insert(21,out.text);    
            size+=results.length();
            //std::cout << results << '\n';

        } else { // if the api request fails, display error msg
            errorCode = 404;
            h_num = H_PLAIN;
            size = SIZE_ERROR;
        }
    }
    
   std::ostringstream oss; // output stream
   oss  <<                 "HTTP/1.1 "<<errorCode<<" OK\r\n"
                            "Content-Type:"<< headers[h_num] <<"\r\n"
                            "ETag: \"ls-"<< headers[h_num] <<"\"\r\n"
                           "Content-Length: "<< size <<"\r\n"
                           "\r\n";
    
    if(f.is_open() && f.good()){ // write the opened file's buffer into ostringstream
        if(out.status_code == 200){ // if search happened, append script to the beginning
            oss<< results;
        }
        if(h_num == H_JSON){ // if json, imitate a json document
        } else { // if not json (most of the time)
            oss << f.rdbuf();
            f.close(); // close the file
            
        }
    }else if(h_num == H_JSON){
        oss << parsed;
            
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

