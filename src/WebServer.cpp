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


// Helper function to compress a string
std::string compressGzip(const std::string& str) {
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, 6) != Z_OK)
        throw(std::runtime_error("deflateInit (level 9) failed while compressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string out;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (out.size() < zs.total_out) {
            // append the block to the output string
            out.append(outbuffer,
                             zs.total_out - out.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return out;
}

unsigned int lastWord(const std::string& word){
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
   unsigned long size;
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
            std::cout << parsed.substr(lastWord(parsed)) << '\n';
            parsed = std::string("[\"")+ (parsed.c_str()+4) + "\", [\"hello linkedin!\"]]";
            errorCode = 200;
            size = parsed.length();
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
        } else if (h_num != H_PLAIN && h_num != H_JSON){
            parsed.insert(0,"www");
            f.open(parsed);
        }
        
        //std::cout << parsed << '\n';
        // if file-opening requests are good, record size and error code
        if( h_num == H_JSON || (f.is_open() && f.good())){ // checks if json or file is open
            errorCode = 200;
            // size is the size of header, size of parsed file, and 
            //size = std::filesystem::file_size(parsed);
        }
        //std::cout << h_num << '\t' << parsed<< '\n';
        
    }
    

    std::string oss; // output string
    if(h_num == H_JSON){ // if a json ( not a file)
        oss = parsed;
    } else if(h_num == H_PLAIN){ // error ! plain file only on error 404
        oss = "error 404";
    } else if (f.is_open() && f.good()) { // if file is all good, (html, css, js. in a search it does this too)
        oss = std::string(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>()
        );
        f.close();
    }


    // can append the results at the beginning once other string operations were concluded
    cpr::Response out;
    if(resp.valid()){ // a search is going on!!
        std::cout << "waiting.." << '\n';
        out = resp.get(); // wait for response
        std::cout << "wait DONE" << '\n';

        if(out.status_code == 200){ // everything worked out
            //results.insert(21,out.text);  
            // insert compressed results to the beginning of the output response
            oss.insert(0,std::format("<script>let results ={};</script>",out.text));
            //size+=results.length();
            //std::cout << results << '\n';

        } else { // if the api request fails, display error msg
            h_num = H_PLAIN;
            errorCode = 404;
            oss = "error 404";
        }
    }
    std::cout << oss << '\n';
    
    oss = compressGzip(oss);
    size = oss.size();
//    std::ostringstream oss; // output stream
    oss.insert(0,             std::format("HTTP/1.1 {} OK\r\n"
                            "Content-Encoding: gzip\r\n"
                            "Cache-Control: max-age=31536000\r\n"
                            "Content-Type:{}\r\n"
                            "ETag: \"ls-{}\"\r\n"
                           "Content-Length: {}\r\n"
                           "\r\n", errorCode, headers[h_num], headers[h_num], size));
    
    //std::cout << oss.str() << '\n';            
    //oss<< f.rdbuf(); // copy buffer from filestream to stringstream
    std::cout << oss << '\n';

   sendToClient(client, oss.c_str(), oss.length()); // send to client
}; 

void WebServer::onClientConnected(int client){


};

void WebServer::onClientDisconnected(int client){


}; 

