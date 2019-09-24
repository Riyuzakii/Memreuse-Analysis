import sys, os
import numpy as np 
import matplotlib.pyplot as plt
fig = plt.figure()
ax = plt.axes()

filename = '../traces/q2outaddrtrace'+sys.argv[1]+'.out'
print filename
file = np.loadtxt(filename, delimiter=': ', dtype=float, usecols=(1))
xaxis = np.loadtxt(filename, delimiter=': ', dtype=float, usecols=(0))

# print len(file)
# f_dist = []
print file
# sys.exit(0)
x = np.linspace(0, len(file), len(file))
# N = file[len(file)-1]

print file
plt.plot(xaxis, file)
# plt.xscale('linear')
plt.xscale('log')
title = 'Q2addtrace'+sys.argv[1]
fig.suptitle(title)
plt.xlabel('Access Distance')
plt.ylabel('Cumulative Distance Function')
plt.savefig(title+'.pdf')
# plt.show()