from header import *
import tldextract
import random
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


#df = pd.read_parquet("hf://datasets/bstds/job_titles/data/train-00000-of-00001-f3966556d39a54a6.parquet") # load the dataset
from datasets import load_dataset


# 1st dataset , 
#ds1 = [element.replace('/', '') for element in list(load_dataset("shreyasharma/sentences_truthv2", split="train")['sentences'])] # truth nuke questions

# dictionary: will contain a bunch of words !! for autocomplete
#dictionary = {load_dataset("jeggers/words_length_short",split="train")['word']} # short words
dictionary = set()
# dictionary.update([word.lower() for word in load_dataset("AIGym/top-100K-words", split="train")['text'] if word.isalnum()]) # top 100k words
# #dictionary.update([word.lower() for word in list(load_dataset("mmathys/profanity", split="train")['text']) if word.isalnum() and not ' ' in word]) # adds profanity. unsure if spaces are present, removed just in case
# dictionary.update(load_dataset("sunildkumar/popular_english_words",split="train")['word'])
so_stack = load_dataset("pacovaldez/stackoverflow-questions",split="train", streaming=True) # stack overflow questions
so_stack.shuffle(seed=random.randint(0,1000), buffer_size=10000) # shuffle the dataset
ds_websites = load_dataset("arcadia1991/top-1M-website",split="train", streaming=True)

async def reroute(url):
    # Ensure the URL has a scheme (http/https) for accurate parsing
    if not url.startswith(('http://', 'https://')):
        url = 'https://' + url
    try:
        response = await requests.head(url, allow_redirects=False)
        if 300 <= response.status_code < 400:
            parsed_url = urlparse(response.headers['Location'])
        else:
            return ""
        # netloc gives 'www.example.com', replace 'www.' if needed
        return parsed_url.netloc.replace('www.', '', 1)
    except:   
        return ""


# # WEBSITES DATASET PREPARATION
# websites = list(['google.com'] + list(ds_websites.take(10000)['google.com'])) # top 100000 websites, for some reason google is the column name 
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


# dictionary.update([key+'.'+value for key,value in sites_visited.items()]) # websites could have duplicate reroutes and it will be handled by the set


# STACK OVERFLOW QUESTIONS PREPARATION
def is_number(s:str):
    return is_num(s) and is_num(s[1:]) and is_num(s[:-1])

def is_num(s:str):
    return s.replace('.','').isdigit()
    
# do some operations on the entire dictionary
#so = list(set([ word for title in list(so_stack.take(25000)['title']) for word in title.split()])) 
so = list(so_stack.take(25000)['title']) # stack overflow questions, each string is a question


for sentence in so: # for every sentence
    words = str(sentence).split() # split by word
    num_words = len(words) # get length 
    
    
    i = 0
    while(i<num_words):
        curr = words[i]
        curr= curr.lower()

        if(not curr.isascii() or curr.count('/')> 1 or curr.count('\\')>0): # if a file path (most have more than 1 directory) or on WINDOWS (ew)
            continue
        
        # curr = curr.encode('ascii') # to lowercase and encodes in ascii
        # curr = curr.decode()
        prev = ''


        # if no changes were made, curr == prev, thus stop editing
        while(curr != prev): 
            prev = curr   
            #stripboth(curr,'(',')') # removes anything in parenthese
            if (curr.startswith('(') and curr.endswith(')')): 
                curr.removeprefix('(').removesuffix(')')
            curr = curr.lstrip('\"\'`,!?;:=') # dont remove . ( methods)
            curr = curr.rstrip('\"\'`.!?,;:=') # done remove closing parentheses (could be a function)
            # start of sentence is usually methods, shouldnt be toucheed
            curr = curr.strip('\"\'`') # strip quotes
            # left_ctr = curr.count('(') 
            # right_ctr = curr.count(')')
            # if(left_ctr>right_ctr):  # opens parentheses
            #     curr.replace('(','',1)
            # elif (right_ctr>left_ctr): # closes parentheses
            #     idx = curr.rfind(')')
            #     curr = curr[:idx] + curr[idx+1:]

        """
        could be the beginning of a function call, ? 
        how to handle? iterate 5 indeces 
        """

        if(len(curr)>2 and not is_number(curr)): # if new word is more than 2 chars and not number , 
            dictionary.add(curr)

        i+=1
        

#ds1 += [re.sub(r'[^\x00-\x7F]+', '',word) for word in list(load_dataset("lighteval/natural_questions_clean", split="train")['question'])] #question words


# 2nd dataset imports as utf 8, have to convert to ascii by removing the invalid characters
#titles = [name for name in ds1 if len(name)>2]
#dictionary = list(set([ word for title in titles for word in title.split()]))
#print(dictionary[:10])
#dictionary = list(set([ word for title in titles for word in title.split()]))
iword = {i:word for i, word in enumerate(dictionary)}
stop_word = '\\'
stop_wordi = len(dictionary)
iword[stop_wordi] = stop_word

wordi = {word:i for i, word in iword.items()}



embedding_dimensions = 10 # number of integers to repsent in n dimension space
dictionary_size = len(dictionary)+1
block_size = 10
hidden_layer_size = 3000
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

def visualize(x,y, num):
    for i in range(num):
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

    
    visualize(x,y,1000)
    print(x.shape)
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
    print("Model and related variables saved successfully.")
