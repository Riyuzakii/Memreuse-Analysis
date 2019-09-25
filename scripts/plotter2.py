import sys, os
import numpy as np 
import matplotlib.pyplot as plt
# from pathlib import Path
directory = "../trace2/"
for root, dirs, files in os.walk(directory):
	for file in files:
		fig = plt.figure()
		ax = plt.axes()
		print file
		# sys.exit(0)
		filename = directory+ file
		print filename
		file_data = np.loadtxt(filename, delimiter=': ', dtype=float, usecols=(1))
		xaxis = np.loadtxt(filename, delimiter=': ', dtype=float, usecols=(0))

		x = np.linspace(0, len(file_data), len(file_data))
		plt.plot(xaxis, file_data)
		plt.xscale('log')
		text = filename
		part = text[11:12]
		prog = text[-5:-4]
		print part, prog
		title = "Part"+part+ ": Cumulative distance v/s Access distance for prog"+prog+".c"
		fig.suptitle(title)
		plt.xlabel('Access Distance')
		plt.ylabel('Cumulative Distance Function')
		plt.savefig(text[10:-4]+'.pdf')
		# plt.show()