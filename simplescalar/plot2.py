
import matplotlib.pyplot as plt
import numpy as np


file_name = "./logs/EnergyEfficiency.log"

file = open(file_name, 'r')
y_axis = []
x_axis = []
x = 1
for line in file.readlines():
    temp = line.rstrip().split(',')[0] #using rstrip to remove the \n
    y_axis.append(float(temp))
    x_axis.append(x)
    x+=1
    print(temp)
    
plt.plot(y_axis)
plt.xticks(np.arange(0, max(x_axis)+1, 10.0))
plt.yticks(np.arange(0, 1.1, 0.1))
axes = plt.gca()
axes.yaxis.grid()
plt.ylabel("Normalized Geomean EDP")
plt.xlabel("Number of Designs Considered")
plt.show()
