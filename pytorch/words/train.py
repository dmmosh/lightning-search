from header import *
<<<<<<< HEAD
import tldextract
import random
=======

>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
'''
train the nn model

start from scratch and save the model at the end
'''
import requests
from urllib.parse import urlparse
# set to cuda or cpu
if torch.cuda.is_available():
    torch.set_default_device('cuda')
    print("Default device set to CUDA")
else:
    torch.set_default_device('cpu')
    print("CUDA not available, default device set to CPU")
    
torch.serialization.add_safe_globals([torch.nn.modules.container.Sequential])   # sequential copntainer as safe


<<<<<<< HEAD
#df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
from datasets import load_dataset
checkpoint = torch.load('model_states_sentences.pth') # going to copy the dictionary and wordi / iword values from sentences 
iword = checkpoint['iword']
wordi = checkpoint['wordi']
stop_wordi = checkpoint['stop_wordi']
stop_word=iword[stop_wordi]

# 1st dataset , 
#ds1 = [element.replace('/', '') for element in list(load_dataset("shreyasharma/sentences_truthv2", split="train")['sentences'])] # truth nuke questions

# dictionary: will contain a bunch of words !! for autocomplete
#dictionary = {load_dataset("jeggers/words_length_short",split="train")['word']} # short words
dictionary = set()
# dictionary.update([word.lower() for word in load_dataset("AIGym/top-100K-words", split="train")['text'] if word.isalnum()]) # top 100k words
# #dictionary.update([word.lower() for word in list(load_dataset("mmathys/profanity", split="train")['text']) if word.isalnum() and not ' ' in word]) # adds profanity. unsure if spaces are present, removed just in case
# dictionary.update(load_dataset("sunildkumar/popular_english_words",split="train")['word'])
ds_stack = load_dataset("pacovaldez/stackoverflow-questions",split="train", streaming=True) # stack overflow questions
ds_stack.shuffle(seed=random.randint(0,1000), buffer_size=10000) # shuffle the dataset
# ds_websites = load_dataset("arcadia1991/top-1M-website",split="train", streaming=True)

# async def reroute(url):
#     # Ensure the URL has a scheme (http/https) for accurate parsing
#     if not url.startswith(('http://', 'https://')):
#         url = 'https://' + url
#     try:
#         response = await requests.head(url, allow_redirects=False)
#         if 300 <= response.status_code < 400:
#             parsed_url = urlparse(response.headers['Location'])
#         else:
#             return ""
#         # netloc gives 'www.example.com', replace 'www.' if needed
#         return parsed_url.netloc.replace('www.', '', 1)
#     except:   
#         return ""


# # WEBSITES DATASET PREPARATION
# websites = list(['google.com'] + list(ds_websites.take(12000)['google.com'])) # top 100000 websites, for some reason google is the column name 
# sites_visited = {} # site : shortest prefix ( list )
# for i in range(len(websites)-1,-1,-1): # remove the overlapping prefixes from websites
#     extracted = tldextract.extract(websites[i])
#     print(f"Subdomain: {extracted.subdomain}") # forums.news
#     print(f"Domain: {extracted.domain}")       # example
#     print(f"Suffix: {extracted.suffix}")       # co.uk
#     print(f"Registered Domain: {extracted.registered_domain}") # example.co.uk
    
#     full = extracted.subdomain +extracted.domain + extracted.suffix
#     dosuff = extracted.domain + extracted.suffix
#     subdom = extracted.subdomain +  extracted.domain
    
#     if full in dictionary:
#         dictionary.remove(full)
#     if dosuff in dictionary:
#         dictionary.remove(dosuff)
#     if subdom in dictionary:
#         dictionary.remove(subdom)
#     if extracted.subdomain in dictionary:
#         dictionary.remove(extracted.subdomain)
#     if extracted.domain in dictionary:
#         dictionary.remove(extracted.domain)
#     if extracted.suffix in dictionary:
#         dictionary.remove(extracted.suffix)
    
#     subdom = extracted.subdomain + '.' if len(extracted.subdomain)>0 else '' +  extracted.domain

    
#     if subdom in sites_visited: # if site has already been visited , look if the suffix count is shorter than already present
#         if(len(str(extracted.suffix).split('.'))< len(sites_visited[subdom].split('.'))): # if amount of suffixes is less than whats already present,
#             sites_visited[subdom] = extracted.suffix
            
#     else: # if site hasnt been visited
        
#         #sites_visited.add(subdom) # add the prefix
#         sites_visited[subdom] = extracted.suffix


#dictionary.update([key+'.'+value for key,value in sites_visited.items()]) # websites could have duplicate reroutes and it will be handled by the set

import re

# SPECIAL CHARACTERS PREPARATION
def stripboth(new:str, strip_start, strip_end=""): # strips both trailing and leading iff they match 
    if(strip_end == ''): # if no arg, copy the strip start but reverse 
        strip_end = strip_start[::-1]
    if(len(strip_start) != len(strip_end)):
        return 
    
    # remove leading and trailing if they match any of the matching chars
    while(new.startswith(strip_start) and new.endswith(strip_end)): 
        new.removeprefix(strip_start).removesuffix(strip_end)

# do some operations on the entire dictionary
for word in set([ word for title in list(ds_stack.take(25000)['title']) for word in title.split()]):
    
    # splits file by separators, DO NOT add file direectories
    separators = list(filter(None,re.split(r'[/\\]', word)))
    if(len(separators) != 1): # if theres MORE than 1 element (most directories), remove it and move on
        continue
    
    new = separators[0] # new word, to be altered
    if(not new.isascii()): # if the word contains non ascii characters, skip it
        continue
    
    
    new = str(new.encode('ascii')).lower() # to lowercase and encodes in ascii
    new = new.rstrip('.,?!;')# removes end of sentence word
    # start of sentence is usually methods, shouldnt be toucheed
    new = new.strip('\"\'') # strip quotes
    stripboth(new,'(',')') # removes anything in parenthese
    new = new.strip('\"\'') # strip quotes again 
    
    left_ctr = new.count('(') 
    right_ctr = new.count(')')
    if(left_ctr>right_ctr):  # opens parentheses
        new.replace('(','',1)
    elif (right_ctr>left_ctr): # closes parentheses
        idx = new.rfind(')')
        new = new[:idx] + new[idx+1:]
    
    if(len(new)>2): # if new word is more than 2 chars, add it
        dictionary.add(new)
     
dictionary.add('c++') # hehe
dictionary.add('to')

#ds1 += [re.sub(r'[^\x00-\x7F]+', '',word) for word in list(load_dataset("lighteval/natural_questions_clean", split="train")['question'])] #question words


# 2nd dataset imports as utf 8, have to convert to ascii by removing the invalid characters
#titles = [name for name in ds1 if len(name)>2]
#dictionary = list(set([ word for title in titles for word in title.split()]))
=======

df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
titles = list(set(df['name']))  # get the titles
dictionary = list(set([ word for title in titles for word in title.split()]))
>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
#print(dictionary[:10])
#dictionary = list(set([ word for title in titles for word in title.split()]))
iword = {i:word for i, word in enumerate(dictionary)}
<<<<<<< HEAD
stop_word = '\\'
=======
stop_word = '?'
>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
stop_wordi = len(dictionary)
iword[stop_wordi] = stop_word

wordi = {word:i for i, word in iword.items()}

<<<<<<< HEAD


embedding_dimensions = 10 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary)+1
block_size = 10
hidden_layer_size = 3000
=======
embedding_dimensions = 5 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary) + 1
block_size = 10
hidden_layer_size = 500
>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
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
    i = 0
    while(i<iterations):
        ix = torch.randint(0,len(y), (batch_size,))  # gives random index 
        #print(y.shape, ix)
        xb, yb = x[ix], y[ix] # random input and output
        # for element in xb:
        #     for e in element:
        #         if(e>=len(embed)):
        #             print("OUT OF BOUNDS ",e, len(embed))
        #print(int(torch.max(xb).item()))
        embedx = embed[xb]
        #print(embedx.shape)
        
        # 512 x 5 x 3 matrix (3d) -> 512 x 15 matrix (2d)
        embedx = embedx.view(embedx.shape[0], embedding_dimensions*block_size)
        
        
        logits = model(embedx)
        
        loss = F.cross_entropy(logits,yb)
        
        # back propagation
        for p in model.parameters():
            p.grad = None
        
        loss.backward()
        
        for p in model.parameters():
            p.data -= learning_rate * p.grad # minimizes loss
        if i%100 == 0:
            print('iteration', i, 'loss:', loss.item())
        i+=1

#print(model)

if __name__ == '__main__':

    x,y = build_data(dictionary)

    
    visualize(x,y,iword)
<<<<<<< HEAD
    print(x.shape)
=======
>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
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
    }, save_path_words)
<<<<<<< HEAD
    print("Model and related variables saved successfully.")
=======
    print("Model and related variables saved successfully.")







>>>>>>> 4141bd7451489f79e93a81d6e7f8c75a8e3489ae
