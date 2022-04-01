
import matplotlib.pyplot as plt
import numpy as np


file_name = "./logs/EnergyEfficiency.best"

file = open(file_name, 'r')
y_axis = []
x_axis = []
x = 1
#file.readlines() # skip first line

line = file.readlines()

temp = line[0].rstrip().split(',') #using rstrip to remove the \n
print(temp)
GMean = float(temp[1])
for index in range(6,15,2):
	y_axis.append(float(temp[index]))

y_axis.append(GMean)
print(y_axis)

x_axis = ['1','2','3','4','5','GMean']

plt.bar(x_axis, y_axis,zorder=3)
#plt.xticks(np.arange(0, max(x_axis)+1, 10.0))
plt.yticks(np.arange(0, 1.1, 0.1))
axes = plt.gca()
axes.yaxis.grid(zorder=0)
plt.ylabel("Normalized EDP")
plt.xlabel("Benchmarks")
plt.show()
