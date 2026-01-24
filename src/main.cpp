#include "WebServer.h"
#include <iostream>
#include <torch/script.h>
//using namespace std;
torch::Device device(torch::kCPU); // global device, cpu 
torch::jit::script::Module module; // the autocorrect model
model_vars_struct model_vars; // the model variables 

void assign_attributes(const torch::jit::script::Module& module) {
    for (const auto& attribute : module.named_attributes()) {
        if(attribute.name == "iword"){
            model_vars.iword = attribute.value.to<torch::Dict<int64_t,std::string>>();
            std::cout << "iword dictionary loaded, size:\t" << model_vars.iword.size() << '\n';
        } else if(attribute.name == "block_size"){
            model_vars.block_size = attribute.value.toInt();
            std::cout << "block size loaded:\t" << model_vars.block_size << '\n';
        } else if(attribute.name == "stop_word"){
            model_vars.stop_word = attribute.value.toInt();
            std::cout << "stop word loaded:\t" << model_vars.stop_word << '\n';

        }
        //std::cout << "Attribute name: " << attribute.name << ", type: " << attribute.value.type()->str() << std::endl;
    }

    if(model_vars.iword.empty()){
        throw std::runtime_error("ERROR iword not loaded!");
    } 
    std::cout<<"Variables loaded successfully.\n";
    // if loop finishes and iword is not found, throw an error
}

int main()
{
    try {
      // Deserialize the ScriptModule from a file using torch::jit::load().
      // note: all inputs and outputs will be in the torch::jit::IValue format, or generic format.. have to be converted to tensors after 
      module = torch::jit::load(MODEL_PATH);
      std::cout << "Model successfully loaded from path\t" << MODEL_PATH << '\n';
    }
    catch (const c10::Error& e) { // if file NOT found, throw an error
        throw std::runtime_error(std::string("ERROR model not found from path\t")+MODEL_PATH +'\n');
      return -1;
    }


    assign_attributes(module); // assign the attributes

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