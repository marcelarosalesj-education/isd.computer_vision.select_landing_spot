from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

#print "Graph name:"
#filename  = raw_input()
filename = "original.txt"
X = np.arange(0, 200, 1.0)
Y = np.arange(0, 200, 1.0)
X, Y = np.meshgrid(X,Y)
Z = np.loadtxt("./maps/"+filename, delimiter="\t", usecols=range(200))


m=(1-0)/(Z.max()-Z.min())
b=1-m*Z.max()
Z = m*Z+b


#import pdb;pdb.set_trace()


fig = plt.figure()
ax = fig.gca(projection='3d')

surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.coolwarm, linewidth=0, antialiased=False)
ax.set_zlim(-0.01, 1.01)

ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()

#import pdb;pdb.set_trace()
