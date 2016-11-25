from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np

def generateSin():
	X = np.arange(0, 200,1.0)
	Y = np.arange(0, 200,1.0)
	X, Y = np.meshgrid(X,Y)
	R = np.sqrt(X**2 + Y**2)
	Z = abs(np.sin(R))
	np.savetxt("./maps/sin.txt", Z, fmt="%.4f", delimiter="\t", newline="\n")
	


generateSin()