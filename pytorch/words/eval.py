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
embed = checkpoint['embed']
embedding_dimensions = checkpoint['embedding_dimensions'] # number of integers to repsent in n dimension space
dictionary_size = checkpoint['dictionary_size']
iword = checkpoint['iword']
wordi = checkpoint['wordi']
hidden_layer_size = checkpoint['hidden_layer_size']
block_size = checkpoint['block_size']
stop_wordi = checkpoint['stop_wordi']
stop_word=iword[stop_wordi]
print("Model and related variables loaded successfully.")

model = nn.Sequential(
    nn.Linear(embedding_dimensions*block_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
model.load_state_dict(checkpoint['model_state_dict'])
model.eval()


def inference(start_word, stopi):
    
    out = start_word[-block_size:].rjust(block_size,stop_word)
    out = [ord(c) for c in out]
    #print(out)
    embedx = embed[torch.tensor(out)]
    embedx = embedx.view(1,embedding_dimensions*block_size)
    #print(embedx.shape)
    with torch.no_grad():
        output = model(embedx)
        probs = torch.softmax(output, dim=1)
        _, top_indices = torch.topk(probs, k=10, dim=1)
        return [iword[i] for i in top_indices.tolist()[0]]


if __name__ == '__main__':
    print('sentence prediction testing')
    while(True):
        
        start_word = input()
        
        out = inference(start_word,stop_wordi)
        # if(start_word not in wordi):
        #     print(start_word,'->')
        #     continue
    
        # out = set()
        # i = 0
        # while(i<50 and len(out)<6):
        #     infer = inference(start_word,stop_wordi)
        #     if(infer != ''):
        #         out.add(infer)
        #     i+=1

        for o in out:
            print(start_word, '-> ', o)
    
    