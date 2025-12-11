#include "WebServer.h"
#include <iostream>
//using namespace std;

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL); 
    
    std::cout<< "running\n";
    WebServer webServer("0.0.0.0", 8080);
    if(webServer.init() != 0){
        return 0;
    }
    webServer.run();
    system("pause");
    
}