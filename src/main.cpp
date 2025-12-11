#include "WebServer.h"
#include <iostream>
//using namespace std;

int main()
{
    std::cout<< "running\n";
    WebServer webServer("127.0.0.1", 8080);
    if(webServer.init() != 0){
        return 0;
    }
    webServer.run();
    system("pause");
    return 0;
}