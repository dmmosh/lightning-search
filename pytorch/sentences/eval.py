import torch
from header import * # read access
import re
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
stop_word=iword[stop_wordi]
print("Model and related variables loaded successfully.")

model = nn.Sequential(
    nn.Linear(embedding_dimensions*block_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
model.load_state_dict(checkpoint['model_state_dict'])
model.eval()


def inference(start_word):
    context = [wordi[start_word]]*block_size
    words = []
    
    while(len(words) < 5):
        embedx = embed[torch.tensor(context)]
        embedx = embedx.view(1,embedding_dimensions*block_size)
        
        logits = model(embedx)
        #print(logits)
        probs = F.softmax(logits,dim=1)
        chari=torch.multinomial(probs,num_samples=1).item()
        top_probs, top_indices = torch.topk(probs, k=6, dim=1)
        print([iword[i] for i in top_indices.tolist()[0]])
        #print(iword[chari])
        context = context[1:] + [chari]
        words.append(iword[chari])
        if(chari == stop_wordi):
            break
        
    #print(words)
    return ' '.join(words[:-1])


def inference(start_word:str):
    sentence = str(re.split(r'[.,?!\'\"]','',start_word)[-1]) # gets the LAST sentence
    slide = sentence.split(' ')[-block_size:] # splits the last sentence into a list of last block size words
    if slide[-1] not in wordi:
        return ['no matches']
    # last element guranteed to be ready for dataset
    slide = ['']*(block_size-len(slide))+ slide # pad the slide with invalid values
    
    
    # last word guranteed to be in the list , iterate back to get more
    # if an invalid word is found, replace it with the last valid
    last_valid = slide[-1]
    
    for i in range(len(slide)-2,-1,-1): # start at 2nd to last element, if one element it doesnt iterate
        if(slide[i] in wordi):
            last_valid = slide[i]
        else:
            slide[i] = last_valid
    
    print(slide)
    
    
    
    
    with torch.no_grad():
        slide = [wordi[start_word]]*block_size
        embedx = embed[torch.tensor(slide)]
        embedx = embedx.view(1,embedding_dimensions*block_size)
        #print(embedx.shape)
        output = model(embedx)
        probs = torch.softmax(output, dim=1)
        _, top_indices = torch.topk(probs, k=10, dim=1)
        out = [iword[i] for i in top_indices.tolist()[0]] # there shouldnt be any duplicates
        
        
        # for every follow up 
        for i, follow_up in enumerate(out):
            
            slide = [wordi[start_word]]*block_size
            slide[-1] = wordi[follow_up]
            j = 0
            while(j<15):
                embedx = embed[torch.tensor(slide)]
                embedx = embedx.view(1,embedding_dimensions*block_size)
                output = model(embedx)
                probs = torch.softmax(output, dim=1)
                _, top_indice = torch.max(probs,dim=1)
                
                top_word = iword[top_indice.item()]
                if(top_word == stop_word):
                    break
                
                out[i] += ' ' + top_word
                
                slide = slide[1:] + [wordi[top_word]] # moves the slide forward
                j+=1
            
            # j = 0
            # top_word = follow_up
            # while(j<15 and top_word != stop_word): # wont iterate if follow_up is stop_word
                
                
                
            #     if(top_word == iword[top_indice.item()]):
            #         break
            #     out[i] += ' ' + top_word # adds the top word (on 1st iteration itll add)
                
            #     j+=1

                # if top_word == stop_word or top_word == out[i]: # if the top word is stop or same word, 
                #     continue
                # out[i] += ' ' + top_word
            
    
    return out

if __name__ == '__main__':
    print('sentence prediction testing')
    while(True):
        
        start_word = input()
        if(start_word not in wordi):
            print(start_word,'->')
            continue
    
        infer = inference(start_word)
        print(infer)
    
    