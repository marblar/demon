import csv
from mpl_toolkits.mplot3d import Axes3D
import numpy
import matplotlib
import matplotlib.pyplot as plt
import itertools
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
from math import log
from math import atan as arctan
from sys import stdin

reader = csv.reader(stdin)
values = itertools.chain(reader)
header = values.next()

x, y, z = [], [], []
for k in values:
    map(lambda a: a[0].append(float(a[1])),zip((x,y,z),k))

for k in zip(x,y,z):
    print k

RYB = plt.get_cmap('RdYlBu')

def colorBy(x):
    return 1-abs(1-x)

def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

col = numpy.array(map(RYB,map(colorBy,z)))

fig = plt.figure()
ax = Axes3D(fig)

p = ax.scatter(x,y,z,s=12,vmin=-1,vmax=1,c=col, cmap=RYB)
fig.add_axes(ax)
m = cm.ScalarMappable(cmap=RYB)
m.set_array(col)
fig.colorbar(m,boundaries=numpy.arange(0,2,.01),values=numpy.array(map(colorBy,drange(0,2,.01))[:-1]))

ax.set_xlim(.5,1)
ax.set_ylim(0,1)
ax.set_zlim(-1,5)

ax.set_xlabel(u"\u2202")
ax.set_ylabel(u'\u03F5')
ax.set_zlabel('<J>')

print "Axes:"
print "%s: (%s)" % (header[0],ax.get_xlim())
print "%s: (%s)" % (header[1],ax.get_ylim())
print "%s: (%s)" % (header[2],ax.get_zlim())
#fig.show()
#import time
#time.sleep(10)

fig.savefig("plot.png")
