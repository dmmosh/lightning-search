#include "WebServer.h"
#include <iostream>
#include <torch/script.h>
//using namespace std;
torch::Device device(torch::kCPU); // global device, cpu 
torch::jit::script::Module module;

void print_attributes(const torch::jit::script::Module& module) {
    for (const auto& attribute : module.named_attributes()) {
        std::cout << "Attribute name: " << attribute.name << ", type: " << attribute.value.type()->str() << std::endl;
    }
}

int main()
{
    try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
      // note: all inputs and outputs will be in the torch::jit::IValue format, or generic format.. have to be converted to tensors after 
      module = torch::jit::load(MODEL_PATH);
      std::cout << "Model successfully loaded from path\t" << MODEL_PATH << '\n';
    }
    catch (const c10::Error& e) {
      std::cerr << "error loading the model from path\t" << MODEL_PATH << '\n';
      return -1;
    }

    print_attributes(module);

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