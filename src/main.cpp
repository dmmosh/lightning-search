#include "WebServer.h"

//using namespace std;

int main()
{
    WebServer webServer("127.0.0.1", 8080);
    if(webServer.init() != 0){
        return 0;
    }
    webServer.run();
    system("pause");
    return 0;
}