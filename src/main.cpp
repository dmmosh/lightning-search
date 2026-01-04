#include "WebServer.h"
#include <iostream>
//using namespace std;

int main()
{
    s1 = singleton::getInstance();
    s1->set("hellooo");
    WebServer webServer("0.0.0.0", 8080);
    if(webServer.init() != 0){
        return 0;
    }
    std::cout<< "running\n";
    webServer.run();
    //system("pause");
    
}