from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np

def generateSin():
	X = np.arange(0, 199, 1.0)
	Y = np.arange(0, 199, 1.0)
	X, Y = np.meshgrid(X,Y)
	R = np.sqrt( (X+3))
	super_threshold_indices = R < -1
	R[super_threshold_indices] = 0
	Z = abs(np.sin(R))
	np.savetxt("./maps/sin.txt", Z, fmt="%.4f", delimiter="\t", newline="\n")

	fig = plt.figure()
	ax = fig.gca(projection='3d')

	surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)
	ax.set_zlim(-1.01, 1.01)

	ax.zaxis.set_major_locator(LinearLocator(10))
	ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

	fig.colorbar(surf, shrink=0.5, aspect=5)

	plt.show()
	


generateSin()
