import torch
import torch.nn as nn
import torch.nn.functional as F 
import pandas as pd
import os
import re

save_path_words = "model_states_words.pth"
save_path_sentences = "model_states_sentences.pth"


if(not os.path.exists(save_path_words) and not os.path.exists(save_path_sentences)): # if the save paths isnt present, exit
    os._exit()

# set to cuda or cpu
if torch.cuda.is_available():
    torch.set_default_device('cuda')
    print("Default device set to CUDA")
else:
    torch.set_default_device('cpu')
    print("CUDA not available, default device set to CPU")
torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe




words = torch.load(save_path_words) # model for generating words
sentences = torch.load(save_path_sentences) # model for generating sentences

# all these variables are model-specific

#   tensors
#embed = words['embed']
#embed = sentences['embed']

#   ints
#block_size = words['block_size']
#block_size = sentences['block_size']
#embedding_dimensions = words['embedding_dimensions'] # number of integers to repsent in n dimension space
#embedding_dimensions = sentences['embedding_dimensions'] # number of integers to repsent in n dimension space
#hidden_layer_size = words['hidden_layer_size']
#hidden_layer_size = sentences['hidden_layer_size']


# since both use the same dataset, the dictionary should be the same (to reduce memory usage)
dictionary_size = words['dictionary_size']
iword = words['iword']
wordi = words['wordi']
stop_wordi = words['stop_wordi']
stop_word=iword[stop_wordi]



print("Model and related variables loaded successfully.")

model_words = nn.Sequential(
    nn.Linear(words['embedding_dimensions']*words['block_size'], words['hidden_layer_size'], bias=False), nn.BatchNorm1d(words['hidden_layer_size']), nn.Tanh(),
    nn.Linear(words['hidden_layer_size'], words['hidden_layer_size'], bias=False), nn.BatchNorm1d(words['hidden_layer_size']), nn.Tanh(),
    nn.Linear(words['hidden_layer_size'], dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
model_words.load_state_dict(words['model_state_dict'])
model_words.eval()

model_sentences = nn.Sequential(
    nn.Linear(sentences['embedding_dimensions']*sentences['block_size'], sentences['hidden_layer_size'], bias=False), nn.BatchNorm1d(sentences['hidden_layer_size']), nn.Tanh(),
    nn.Linear(sentences['hidden_layer_size'], sentences['hidden_layer_size'], bias=False), nn.BatchNorm1d(sentences['hidden_layer_size']), nn.Tanh(),
    nn.Linear(sentences['hidden_layer_size'], dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
model_sentences.load_state_dict(sentences['model_state_dict'])
model_sentences.eval()


def inference(query):
    sentence = str(re.split(r'[.,?!\'\"]',start_word)[-1]).split(' ') # gets the LAST sentence as a list

    #word = sentence[-1] # gets the last word (to autocomplete)
    # last word is sentence[-1]
    autocomplete = set() # 
    if(sentence[-1] not in iword):
        

# inferences the words . will return the top 2 word inferences 
def inference_words(start_word):
    
    
    out = start_word[-words['block_size']:].rjust(words['block_size'],stop_word)
    out = [ord(c) for c in out]
    #print(out)
    embedx = words['embed'][torch.tensor(out)]
    embedx = embedx.view(1,words['embedding_dimensions']*words['block_size'])
    #print(embedx.shape)
    with torch.no_grad():
        output = model_words(embedx)
        probs = torch.softmax(output, dim=1)
        _, top_indices = torch.topk(probs, k=2, dim=1)
        return top_indices.tolist()

def inference_sentences(word, k): # the word, autocompleted or already present, and number of responses (2 for autocompleted or 10 for already present)
    
    
    slide = sentence.split(' ')[-block_size:] # splits the last sentence into a list of last block size words
    
    if slide[-1] not in wordi:
        return ['no matches']
    # last element guranteed to be ready for dataset
    slide = ['\\']*(block_size-len(slide))+ slide # pad the slide with invalid values
    
    
    # last word guranteed to be in the list , iterate back to get more
    # if an invalid word is found, replace it with the last valid
    last_valid = slide[-1]
    
    for i in range(len(slide)-2,-1,-1): # start at 2nd to last element, if one element it doesnt iterate
        if(slide[i] in wordi):
            last_valid = slide[i]
        else:
            slide[i] = last_valid
    
    print(slide)
    slide = [wordi[word] for word in slide]
    slide_orig = slide[:]
    with torch.no_grad():
        #slide = [wordi[start_word]]*block_size
        embedx = embed[torch.tensor(slide)]
        embedx = embedx.view(1,embedding_dimensions*block_size)
        #print(embedx.shape)
        output = model(embedx)
        probs = torch.softmax(output, dim=1)
        _, top_indices = torch.topk(probs, k=10, dim=1)
        out = [iword[i] for i in top_indices.tolist()[0]] # there shouldnt be any duplicates
        
        
        # for every follow up 
        for i, follow_up in enumerate(out):
            
            slide = slide_orig[1:] + [wordi[follow_up]]
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
    return out


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
    
    