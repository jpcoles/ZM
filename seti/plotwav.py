import sys
from numpy  import fromfile, loadtxt, median, amax, abs
from pylab import figure, plot, show, hist

#N=10000

#wav = fromfile(sys.argv[1], dtype=dtype('1uint16'), count=N)
wav = loadtxt(sys.argv[1])

med = median(wav)
max = amax(abs(wav)-med)
print 'median', med
print 'max', max

figure()
plot(wav, 'k-') #, marker='x')
#plot((wav-med) / max, 'k-') #, marker='x')

figure()
hist(wav, bins=1000)
show()

