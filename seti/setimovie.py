import matplotlib as mpl
from matplotlib import rc
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

test = 0

rc('axes', edgecolor='w')
rc('axes', facecolor='w')
rc('text', color='w')
rc('axes', labelcolor='w')
rc('axes3d', grid=False)
rc('ytick', color='w')
rc('xtick', color='w')
import pylab as pl

setiwav = pl.imread('setiwav.png')
imh,imw = setiwav.shape[:2]
print setiwav.shape

nseconds = 120
fps = 30
dt = 1./ fps
dpi = 64
fs = [1024/dpi,768/dpi]

u = np.linspace(0, 2 * np.pi, 100)
v = np.linspace(0, np.pi, 100)

x = np.outer(np.cos(u), np.sin(v))
y = np.outer(np.sin(u), np.sin(v))
z = np.outer(np.ones(np.size(u)), np.cos(v))

u = np.linspace(0, 2 * np.pi, 100)
cx = 20 * (np.cos(u))
cy = 20 * (np.sin(u))
cz = 30 + np.zeros(np.size(u))

xe = 2 * x 
ye = 2 * y + 20
ze = 2 * z + 30

xb1 = np.array([  0,0])
yb1 = np.array([-30,-2])
zb1 = np.array([-45,-3])

xb2 = np.array([0, 0])
yb2 = np.array([2,30])
zb2 = np.array([3,45])

qoffs = 0.4015
pt = 0.71452
#pt = 0.73 #0.71452

if test:
    pl.ion()

oc = None
for i,t in enumerate(np.arange(qoffs+128*pt,nseconds, dt)):
    f = pl.figure(figsize=fs, dpi=dpi, facecolor='k', edgecolor='k')

    #help(mpl)
    rc('ytick', color='w')
    rc('xtick', color='w')
    ax = f.add_subplot(211)
    ax.yaxis.set_visible(False)
    ax.set_axis_bgcolor('k')
    ax.set_xlim(0,nseconds)
    ax.set_ylim(0,imh-1)
    ax.imshow(setiwav, extent=[0,nseconds,0,imh-1], aspect='auto', interpolation='nearest')
    ax.axvline(t, color='r')
    ax.set_xlabel('Sekunden')
    pl.title('Pulsar PSR B0329+54 Signal')
    #pl.axis('off')
    #pl.box()
    pl.subplots_adjust(0.1,0.1,0.9,0.9)


    #rc('ytick', color='k')
    #rc('xtick', color='k')
    ax = f.add_subplot(223, projection='3d')
    ax.w_xaxis._AXINFO['x']['color'] = (0.4,0.4,0.4)
    ax.w_xaxis._AXINFO['y']['color'] = (0.4,0.4,0.4)
    ax.w_xaxis._AXINFO['z']['color'] = (0.4,0.4,0.4)
    ax.set_axis_bgcolor('k')
    #ax.w_xaxis.set_major_locator(mpl.ticker.FixedLocator([-20]));
    #ax.w_yaxis.set_major_locator(mpl.ticker.FixedLocator([20]));
    #ax.w_zaxis.set_major_locator(mpl.ticker.FixedLocator([0]));
    ax.w_zaxis.line.set_color((0,0,0))
    ax.w_xaxis.line.set_color((0,0,0))
    ax.w_yaxis.line.set_color((0,0,0))
    #ax.w_xaxis.set_major_locator(mpl.ticker.LinearLocator(1));
    #ax.w_yaxis.set_major_locator(mpl.ticker.LinearLocator(1));
    #ax.w_zaxis.set_major_locator(mpl.ticker.LinearLocator(1));


    q = 2*np.pi * (t-qoffs)/pt #0.71452
    x1 = .5 * (x*np.cos(q) - y*np.sin(q))
    y1 = .5 * (x*np.sin(q) + y*np.cos(q))
    z1 = .5 * z

    xb = (xb1*np.cos(q) - yb1*np.sin(q))
    yb = (xb1*np.sin(q) + yb1*np.cos(q))
    zb = zb1
    ax.plot(xb,yb,zb, color='y', lw=3, alpha=0.5)

    xb = (xb2*np.cos(q) - yb2*np.sin(q))
    yb = (xb2*np.sin(q) + yb2*np.cos(q))
    zb = zb2
    ax.plot(xb,yb,zb, color='y', lw=3, alpha=1.0)

    ax.plot(cx,cy,cz, color='y', lw=1, alpha=1.0)

    c = int((t-qoffs) / pt)
    if oc is not None and c != oc:
        oc = c
        c = '#FFFFFF'
    else:
        if oc is None:
            oc = c
        c = '#00CCCC'

    ax.plot_surface(x1, y1, z1,  rstride=40, cstride=40, color='#CCCCCC', edgecolor='w', shade=False, alpha=0.5)
    ax.plot_surface(xe, ye, ze,  rstride=10, cstride=10, color=c, edgecolor='#007777', shade=False)
    #ax.plot_surface(xe, ye, ze,  rstride=10, cstride=10, color='#00CCCC', edgecolor='#007777', shade=False)


    ax.set_xlim3d(xmin=-20,xmax=20)
    ax.set_ylim3d(xmin=-20,xmax=20)
    ax.set_zlim3d(xmin=0,xmax=40)
    pl.savefig('sm%05i.png' % i, facecolor='k')
    print i
    #ax.w_xaxis._AXINFO['color'] = (0,0,0)
    if test:
        pl.draw()
        raw_input()
    pl.close('all')
    #break


