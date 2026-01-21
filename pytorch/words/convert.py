import torch
from header import * # read access

if(not os.path.exists(save_path_words)): # if the save path isnt present, exit
    os._exit()

# set to cuda or cpu
if torch.cuda.is_available():
    torch.set_default_device('cuda')
    print("Default device set to CUDA")
else:
    torch.set_default_device('cpu')
    print("CUDA not available, default device set to CPU")
torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe

checkpoint = torch.load(save_path_words)


class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.checkpoint = torch.load(save_path_words)
        self.embed = checkpoint['embed']
        self.embedding_dimensions = checkpoint['embedding_dimensions'] # number of integers to repsent in n dimension space
        self.dictionary_size = checkpoint['dictionary_size']
        self.iword = checkpoint['iword']
        self.wordi = checkpoint['wordi']
        self.hidden_layer_size = checkpoint['hidden_layer_size']
        self.block_size = checkpoint['block_size']
        self.stop_wordi = checkpoint['stop_wordi']
        self.stop_word=self.iword[self.stop_wordi]

        self.model = nn.Sequential(
            nn.Linear(self.embedding_dimensions*self.block_size, self.hidden_layer_size, bias=False), nn.BatchNorm1d(self.hidden_layer_size), nn.Tanh(),
            nn.Linear(self.hidden_layer_size, self.hidden_layer_size, bias=False), nn.BatchNorm1d(self.hidden_layer_size), nn.Tanh(),
            nn.Linear(self.hidden_layer_size, self.dictionary_size, bias=False), nn.BatchNorm1d(self.dictionary_size)
        )
        self.model.load_state_dict(checkpoint['model_state_dict'])
        self.model.eval()
        print("Model and related variables loaded successfully.")
        #self.model.eval()
    def forward(self,x):
        #print(out)
        embedx = self.embed[torch.tensor(x)]
        embedx = embedx.view(1,self.embedding_dimensions*self.block_size)
        #print(embedx.shape)
        with torch.no_grad():
            logits = self.model(embedx)
            return logits
        
model = NeuralNetwork()

if __name__ == '__main__': # convert to scriptmodule
    test = 'software'
    out = test[-model.block_size:].rjust(model.block_size,model.stop_word)
    out = [ord(c) for c in out]
    print(out)
    logits = model.forward(out)
    probs = torch.softmax(logits, dim=1)
    _, top_indices = torch.topk(probs, k=10, dim=1)
    
    print([model.iword[i] for i in top_indices.tolist()[0]])
    
    MODEL_NAME = 'www/libtorch_ac_model.pt'
    
    model_scripted = torch.jit.script(model)
    model_scripted.save(MODEL_NAME)
    print('model saved to',MODEL_NAME)
    
    
    
    