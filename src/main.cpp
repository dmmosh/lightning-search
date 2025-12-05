#include "MultiHeader.h"

//using namespace std;

int main()
{
    MultiClientChat mcc("127.0.0.1", 54000);
    if(mcc.init() != 0){
        return 0;
    }
    mcc.run();
    return 0;
}