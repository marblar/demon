import csv
from mpl_toolkits.mplot3d import Axes3D
import numpy
import matplotlib

# Run without the window server.
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import itertools
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
from math import log
from math import atan as arctan
from sys import stdin
import argparse



parser = argparse.ArgumentParser()
parser.add_argument("name")
args = parser.parse_args()

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

fig = plt.figure(figsize=(10,5))
fig.suptitle(args.name,fontsize=14,fontweight='bold')
ax = fig.add_subplot(111, projection='3d')

ax.set_title('<J>=%s' % round(numpy.mean(z),2),position=(0,0),ha='center')

p = ax.scatter(x,y,z,s=12,vmin=-1,vmax=1,c=col, cmap=RYB)
fig.add_axes(ax)
m = cm.ScalarMappable(cmap=RYB)
m.set_array(col)
fig.colorbar(m,boundaries=numpy.arange(0,2,.01),values=numpy.array(map(colorBy,drange(0,2,.01))[:-1]))

ax.set_xlim3d(.5,1)
ax.set_ylim3d(0,1)
ax.set_zlim3d(0,5)

ax.set_xlabel(u"\u2202")
ax.set_ylabel(u"\u0395")
ax.set_zlabel('J')

print "Axes:"
print "%s: (%s)" % (header[0],ax.get_xlim3d())
print "%s: (%s)" % (header[1],ax.get_ylim3d())
print "%s: (%s)" % (header[2],ax.get_zlim3d())
#fig.show()
#import time
#time.sleep(10)



fig.savefig("plot.png")
