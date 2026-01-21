#include "WebServer.h"
#include <iostream>
#include <filesystem>

//using namespace std;
torch::Device device(torch::kCPU); // global device, cpu 



int main()
{
    if(!std::filesystem::exists(MODEL_PATH)){
        std::cout << "Model not found. Autocomplete not possible.\n";
        return 0;
    }
    
    if(torch::cuda::is_available()){ // if cuda is available, switch the global device
        device = torch::Device(torch::kCUDA);
    }
    std::cout << "The server is using:\t"<< (device.is_cuda() ? "CUDA" : "CPU") << '\n';
    

    WebServer webServer("0.0.0.0", PORT);
    if(webServer.init() != 0){
        return 0;
    }
    std::cout<< "running on port " << PORT << '\n'; 
    webServer.run();
    //system("pause");
}