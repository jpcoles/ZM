import sys
from numpy  import fromfile, ceil, mean, abs, floor, reshape, dtype, array
from pylab import figure, plot, show

def save_to_wav(fname, data):
    import numpy as N
    import wave

    # let's prepare signal
    #duration = 4 # seconds
    #samplerate = 100 # Hz
    samplerate = 44100 # Hz
    #samples = duration*samplerate
    #frequency = 440 # Hz
    #period = samplerate / float(frequency) # in sample points
    #omega = N.pi * 2 / period

    #xaxis = N.arange(int(period),dtype = N.float) * omega
    #ydata = 16384 * N.sin(xaxis)

    duration = len(data) / samplerate

    print duration

    #signal = N.resize(ydata, (samples,))

    #ssignal = ''
    #for i in range(len(signal)):
       #ssignal += wave.struct.pack('h',signal[i]) # transform to binary

    #ssignal = ''.join(data) #(wave.struct.pack('h', item) for item in signal))

    #print ssignal

    file = wave.open(fname, 'wb')
    file.setparams((1, 2, samplerate, samplerate*duration, 'NONE', 'noncompressed'))
    d = array(data, 'uint16')
    print d
    file.writeframes(d)
    file.close()

Npeaks = 4
N = ceil((8738133.0 * 0.714519) * Npeaks)
Nbins = 100 * Npeaks
avg = floor(N/Nbins)

print N, Nbins, avg

sys.exit(0)

data = fromfile(sys.argv[1], dtype=dtype('2int8'), count=N)

data = data[:,0] + data[:,1]*1j
power = abs(data) ** 2

del data


power = power[:avg*Nbins]
print power.shape, power.size, power.size/avg, avg
power = reshape(power, (power.size/avg, avg))
power = mean(power, 1)

power *= 200

print power

save_to_wav('power.wav', power)

figure()
plot(power, 'k-')
show()

