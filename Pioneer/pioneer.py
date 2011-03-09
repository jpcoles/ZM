from pylab import plot, show, figure, imread, imshow, clf, savefig, xlim, ylim, axhline, figimage, subplot
from numpy import mean, ones
import numpy

def smooth(x,window_len=11,window='hanning'):
    """smooth the data using a window with requested size.
    
    This method is based on the convolution of a scaled window with the signal.
    The signal is prepared by introducing reflected copies of the signal 
    (with the window size) in both ends so that transient parts are minimized
    in the begining and end part of the output signal.
    
    input:
        x: the input signal 
        window_len: the dimension of the smoothing window; should be an odd integer
        window: the type of window from 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'
            flat window will produce a moving average smoothing.

    output:
        the smoothed signal
        
    example:

    t=linspace(-2,2,0.1)
    x=sin(t)+randn(len(t))*0.1
    y=smooth(x)
    
    see also: 
    
    numpy.hanning, numpy.hamming, numpy.bartlett, numpy.blackman, numpy.convolve
    scipy.signal.lfilter
 
    TODO: the window parameter could be the window itself if an array instead of a string   
    """

    if x.ndim != 1:
        raise ValueError, "smooth only accepts 1 dimension arrays."

    if x.size < window_len:
        raise ValueError, "Input vector needs to be bigger than window size."


    if window_len<3:
        return x


    if not window in ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
        raise ValueError, "Window is on of 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'"


    s=numpy.r_[2*x[0]-x[window_len:1:-1],x,2*x[-1]-x[-1:-window_len:-1]]
    #print(len(s))
    if window == 'flat': #moving average
        w=ones(window_len,'d')
    else:
        w=eval('numpy.'+window+'(window_len)')

    y=numpy.convolve(w/w.sum(),s,mode='same')
    return y[window_len-1:-window_len+1]


im = imread('p10c-sb-small.png')

figure()
#for i,s in enumerate(im[2:,:,0]):
    #print i
    #clf()
    #plot(avg)
    #savefig('p10c-%04i.png' % i)

#avg = mean(im[0:2,:,0])
ywin = 20
xwin = 3
for i in range(180-ywin,0,-1):
    print i
    avg = mean((4*im[i:i+ywin,:,0])**2,0)
    avg = smooth(avg,xwin,'flat')
    #print im[i:i+3,:,0]
    #print avg
    clf()
    subplot(211)
    plot(avg)
    ylim(0,4)
    subplot(212)
    imshow(im)
    axhline(i, c='r')
    savefig('p10c-%04i.png' % (180-ywin-i))
    #break

show()

