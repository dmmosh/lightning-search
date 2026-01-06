from header import *

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



df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
titles = list(set(df['name']))  # get the titles
dictionary = list(set([ word for title in titles for word in title.split()]))
#print(dictionary[:10])
iword = {i:word for i, word in enumerate(dictionary)}
stop_word = '.'
stop_wordi = len(dictionary)
iword[stop_wordi] = stop_word

wordi = {word:i for i, word in iword.items()}

embedding_dimensions = 5 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary) + 1
block_size = 10
hidden_layer_size = 500
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
iterations = 50000 # number of iterations of batches to complete one epoch (pass through entire dataset)


def build_data(dictionary):
    x=[]
    y=[]
    
    
    for word in dictionary:
    
        if word not in wordi:
            continue    
        # do a sliding window
        i = max(0,len(word)-block_size) # start
        j = i
        while(j<len(word)):
            out = [ord(stop_word)]*(block_size-(j-i+1)) + [ord(c) for c in word[i:j+1]]
            #print(out)
            x.append(out)
            y.append(wordi[word])
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
        context_words = [chr(i.item()) for i in context]
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
    train_model(x, y)
    torch.save({
        'model_state_dict': model.state_dict(),
        'embed': embed,
        'iword': iword,
        'wordi': wordi,
        'stop_wordi': stop_wordi,
        'hidden_layer_size':hidden_layer_size,
        'block_size':block_size,
        'embedding_dimensions':embedding_dimensions,
        'dictionary_size':dictionary_size
    }, save_path)
    print("Model and related variables saved successfully.")







