#include "MultiHeader.h"

//using namespace std;

int main()
{
    MultiClientChat mcc("0.0.0.0", 54000);
    if(mcc.init() != 0){
        return 0;
    }
    mcc.run();
    return 0;
}