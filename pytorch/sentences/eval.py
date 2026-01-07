import torch
from header import * # read access

if(not os.path.exists(save_path_sentences)): # if the save path isnt present, exit
    os._exit()

# set to cuda or cpu
if torch.cuda.is_available():
    torch.set_default_device('cuda')
    print("Default device set to CUDA")
else:
    torch.set_default_device('cpu')
    print("CUDA not available, default device set to CPU")
torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe

checkpoint = torch.load(save_path_sentences)
embed = checkpoint['embed']
embedding_dimensions = checkpoint['embedding_dimensions'] # number of integers to repsent in n dimension space
dictionary_size = checkpoint['dictionary_size']
iword = checkpoint['iword']
wordi = checkpoint['wordi']
hidden_layer_size = checkpoint['hidden_layer_size']
block_size = checkpoint['block_size']
stop_wordi = checkpoint['stop_wordi']
print("Model and related variables loaded successfully.")

model = nn.Sequential(
    nn.Linear(embedding_dimensions*block_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
model.load_state_dict(checkpoint['model_state_dict'])
model.eval()


def inference(start_word, stopi):
    context = [wordi[start_word]]*block_size
    words = []
    
    while(len(words) < 5):
        embedx = embed[torch.tensor(context)]
        embedx = embedx.view(1,embedding_dimensions*block_size)
        
        logits = model(embedx)
        #print(logits)
        probs = F.softmax(logits,dim=1)
        chari=torch.multinomial(probs,num_samples=1).item()
        # top_probs, top_indices = torch.topk(probs, k=6, dim=1)
        # print([iword[i] for i in top_indices.tolist()[0]])
        #print(iword[chari])
        context = context[1:] + [chari]
        words.append(iword[chari])
        if(chari == stopi):
            break
        
    #print(words)
    return ' '.join(words[:-1])


def inference(start_word, stopi):
    context = [wordi[start_word]]*block_size
    words = []
    
    while(len(words) < 5):
        embedx = embed[torch.tensor(context)]
        embedx = embedx.view(1,embedding_dimensions*block_size)
        
        logits = model(embedx)
        #print(logits)
        probs = F.softmax(logits,dim=1)
        chari=torch.multinomial(probs,num_samples=1).item()
        # top_probs, top_indices = torch.topk(probs, k=6, dim=1)
        # print([iword[i] for i in top_indices.tolist()[0]])
        #print(iword[chari])
        context = context[1:] + [chari]
        words.append(iword[chari])
        if(chari == stopi):
            break
        
    #print(words)
    return ' '.join(words[:-1])

if __name__ == '__main__':
    print('sentence prediction testing')
    while(True):
        
        start_word = input()
        if(start_word not in wordi):
            print(start_word,'->')
            continue
    
        out = set()
        i = 0
        while(i<50 and len(out)<6):
            infer = inference(start_word,stop_wordi)
            if(infer != ''):
                out.add(infer)
            i+=1

        for o in out:
            print(start_word, '-> ', o)
    
    