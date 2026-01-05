from header import *
import numpy as np 
import header

'''
train the nn model

start from scratch and save the model at the end
'''

# set to cuda or cpu
if torch.cuda.is_available():
    torch.set_default_device('cuda')
    print("Default device set to CUDA")
else:
    torch.set_default_device('cpu')
    print("CUDA not available, default device set to CPU")
    
torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe


def subi(sub:str): # substring to index 
    return int.from_bytes(sub.encode('utf-8'), 'big')

def isub(i): # index to substring
    return i.to_bytes((i.bit_length() + 7) // 8, 'big').decode('utf-8')


df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
titles = list(set(df['name']))  # get the titles
dictionary = list(set([ word for title in titles for word in title.split()]))
#print(dictionary[:10])
iword = {i:word for i, word in enumerate(dictionary)}
stop_word = '.'
stop_wordi = len(dictionary)
iword[stop_wordi] = stop_word
percentile = int(np.percentile(list(map(len,dictionary)),25)) # 25th percentile of dictionary lengths 

wordi = {word:i for i, word in iword.items()}

embedding_dimensions = 5 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary) + 1

block_size = percentile # a good balance between too much and too little
hidden_layer_size = 100
# model
model = nn.Sequential(
    nn.Linear(embedding_dimensions*block_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, hidden_layer_size, bias=False), nn.BatchNorm1d(hidden_layer_size), nn.Tanh(),
    nn.Linear(hidden_layer_size, dictionary_size, bias=False), nn.BatchNorm1d(dictionary_size)
)
embed = torch.rand((dictionary_size, embedding_dimensions)) # embedding layer init, transforms discrete data (words) into numerical vectors

# model training variables
batch_size = 512 # batch size ( amount of samples before the weights are updated)
learning_rate = 0.1 # how much model weights adjust during training
iterations = 100000 # number of iterations of batches to complete one epoch (pass through entire dataset)


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
            print([iword[i] for i in context])
            print(word)
            context = context[1:] + [wordi[word]]
        x.append(context)
        y.append(stop_wordi)
    
    print(x)
    x= torch.tensor(data=x,dtype=torch.long)
    y = torch.tensor(data=y,dtype=torch.long)
    return x,y

def build_data(dictionary):
    x=[]
    y=[]
    
    
    for word in dictionary:
        context = [stop_wordi]*percentile
        wordi = word[:-1]
        
        # do a sliding window
        i = max(0,len(wordi)-percentile) # start
        j = i
        while(j<len(wordi)):
            x.append([stop_word]*(percentile-(j-i+1)) + list(wordi[i:j+1]))
            y.append(word)
            j+=1
        

        
            
            
        
        
        # context = [stop_wordi]*percentile-len(d)-1 if  (percentile-len(d)-1>0) else []
        # context += [d[:i+1] for i in range(len(d)-1-percentile, len(d)-1)]
        # x.append(context)
        # y.append(wordi[word])
        
        
    
    x= torch.tensor(data=x,dtype=torch.long)
    y = torch.tensor(data=y,dtype=torch.long)
    return x,y

def visualize(x,y,iword):
    for i in range(100):
        context = x[i]
        target = y[i]
        context_words = [iword[i.item()] for i in context]
        target_word = iword[target.item()]
        print(f'{context_words} -> {target_word}')

def visualize(x,y,iword):
    for i in range(100):
        context = x[i]
        target = y[i]
        context_words = [isub(i.item()) for i in context]
        target_word = iword[target.item()]
        print(f'{context_words} -> {target_word}')

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


#print(model)

if __name__ == '__main__':

    x,y = build_data(dictionary)
    visualize(x,y,iword)
    
    # train_model(x, y)
    # torch.save({
    #     'model_state_dict': model.state_dict(),
    #     'embed': embed,
    #     'iword': iword,
    #     'wordi': wordi,
    #     'stop_wordi': stop_wordi,
    #     'embedding_dimensions':embedding_dimensions,
    #     'dictionary_size':dictionary_size,
    #     'hidden_layer_size':hidden_layer_size,
    #     'block_size':block_size
    # }, save_path)
    # print("Model and related variables saved successfully.")







