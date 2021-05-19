import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

file1 = open('bcast_results2', 'r')
Lines = file1.readlines()
 
times = [[],[],[],[]]
index = 0
count = 1
for line in Lines:
    times[index].append(float(line.strip()))
    if(count%2 == 0):
        index = (index + 1)%1
    count += 1
len(times[0])
    
sns.set()


# In[48]:


bcast_inp = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})


# In[49]:

index = 0
for execution in range(len(times[0])//24):
    for P in [4, 16]:
        for ppn in [1, 8]:
            for D in [16, 256, 2048]:
                # Change with the actual data
                bcast_inp = bcast_inp.append({
                    "D": D, "P": P, "ppn": ppn, "mode": 0, "time": times[0][index]
                }, ignore_index=True)
                bcast_inp = bcast_inp.append({
                    "D": D, "P": P, "ppn": ppn, "mode": 1, "time": times[0][index+1]
                }, ignore_index=True)
                index += 2
                

bcast_inp["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, bcast_inp["P"]), map(str, bcast_inp["ppn"])))

bcast_inp.to_csv("bcast.csv")

# In[50]:


sns.catplot(x="(P, ppn)", y="time", data=bcast_inp, kind="box", col="D", hue="mode")
plt.savefig("plot_Bcast.jpg")

# In[ ]:




