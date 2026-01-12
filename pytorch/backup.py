import torch
import torch.nn as nn
import torch.nn.functional as F 
import pandas as pd
import os


'''
train the nn model
'''




torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe

df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
titles = list(set(df['name'])) 
dictionary = list(set([ word for title in titles for word in title.split()]))
#print(dictionary[:10])
iword = {i:word for i, word in enumerate(dictionary)}
stop_word = '.'
stop_wordi = len(dictionary)
iword[stop_wordi] = stop_word

embedding_dimensions = 5 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary) + 1

# Save and load loop for the model and related variables
save_path = "model_states.pth"

wordi = {word:i for i, word in iword.items()}

#print(titles[0:10])

def build_data(titles:list[str], iword, wordi,stop_wordi,block_size):
    x=[]
    y=[]
    for title in titles:
        words = title.strip().split(' ')
        context = [stop_wordi]*block_size
        for word in words + [stop_wordi]: # split words with the key at the end
            if word not in wordi: # word is not in dict
                continue
            x.append(context)
            y.append(wordi[word])
            context = context[1:] + [wordi[word]]
        x.append(context)
        y.append(stop_wordi)
    
    x= torch.tensor(data=x,dtype=torch.long)
    y = torch.tensor(data=y,dtype=torch.long)
    return x,y


block_size = 3
# embedding vectors (numerical data to represent the names)



embed = torch.rand((dictionary_size, embedding_dimensions))

def visualize(x,y,iword):
    for i in range(100):
        context = x[i]
        target = y[i]
        context_words = [iword[i.item()] for i in context]
        target_word = iword[target.item()]
        print(f'{context_words} -> {target_word}')

#print(visualize(x,y,iword))
        


# actual model
#hidden_layer_size = int(embedding_dimensions*block_size*2/3) + dictionary_size# 2/3 of size of input + size of output 
hidden_layer_size = 100
model = nn.Sequential(
    nn.Linear(embedding_dimensions*block_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)

batch_size = 512
learning_rate = 0.1
iterations = 100000 # number of iterations
losses = []
def train_model(x,y):
    for i in range(iterations):
        ix = torch.randint(0,x.shape[0], (batch_size,))  # gives random index 
        xb, yb = x[ix], y[ix] # random input and output
        embedx = embed[xb]
        
        # 512 x 5 x 3 matrix (3d) -> 512 x 15 matrix (2d)
        embedx = embedx.view(embedx.shape[0], embedding_dimensions*block_size)
        
        
        logits = model.forward(embedx)
        
        loss = F.cross_entropy(logits,yb)
        
        # back propagation
        for p in model.parameters():
            p.grad = None
        
        loss.backward()
        
        for p in model.parameters():
            p.data -= learning_rate * p.grad # minimizes loss
        if i%1000 == 0:
            print('iteration', i, 'loss:', loss.item())
            losses.append(loss.item())


    




def inference(sample_count, start_word, stopi):
    model.eval()
    samples_out = []
    for i in range(sample_count):
        context = [wordi[start_word]]*block_size
        words = []
        
        while(len(words) < 5):
            embedx = embed[torch.tensor(context)]
            embedx = embedx.view(1,embedding_dimensions*block_size)
            
            logits = model(embedx)
            #print(logits)
            probs = F.softmax(logits,dim=1)
            chari=torch.multinomial(probs,num_samples=1).item()
            #print(iword[chari])
            context = context[1:] + [chari]
            words.append(iword[chari])
            if(chari == stopi):
                break
            
        #print(words)
        samples_out.append(' '.join(words[:-1]))
    
    return samples_out


    
if __name__ == '__main__':
    
    # set to cuda or cpu
    if torch.cuda.is_available():
        torch.set_default_device('cuda')
        print("Default device set to CUDA")
    else:
        torch.set_default_device('cpu')
        print("CUDA not available, default device set to CPU")
    
    
    # train model or     
    if os.path.exists(save_path):
        checkpoint = torch.load(save_path)
        model.load_state_dict(checkpoint['model_state_dict'])
        embed = checkpoint['embed']
        iword = checkpoint['iword']
        wordi = checkpoint['wordi']
        stop_wordi = checkpoint['stop_wordi']
        print("Model and related variables loaded successfully.")
    else:
        train_model(x, y)
        torch.save({
            'model_state_dict': model.state_dict(),
            'embed': embed,
            'iword': iword,
            'wordi': wordi,
            'stop_wordi': stop_wordi
        }, save_path)
        print("Model and related variables saved successfully.")
        
        
    x,y = build_data(titles,iword,wordi,stop_wordi,block_size)
    
    start_word = 'software'
    samples = inference(20,start_word, stop_wordi)

    for sample in samples:
        print(start_word, '-> ', sample)    




