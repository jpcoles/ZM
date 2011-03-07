import matplotlib as mpl
from matplotlib import rc
from operator import add

mpl.rcParams['toolbar'] = 'None'
rc('font',**{'family':'serif','serif':['Computer Modern Roman']})
#rc('text', usetex=True)
rc('font', size=18)
rc('text', color='w')
rc('grid', color='w')
rc('axes', edgecolor='w')
rc('axes', facecolor='k')
rc('axes', labelcolor='w')
rc('axes', linewidth=2)
rc('ytick', color='w')
rc('xtick', color='w')


from pylab import figure,show,plot, Circle, gca, axis, subplot, \
                  draw, gcf, axes, subplots_adjust, loglog, grid,\
                  xlabel, ylabel, ticklabel_format, tick_params, \
                  xticks, yticks, NullLocator, ScalarFormatter, \
                  LogLocator, FormatStrFormatter,scatter

from pylab import ion, ioff, get_current_fig_manager

from matplotlib.pyplot import subplot2grid

from numpy import arange,sin, array
import csv

from collections import defaultdict

def readplanets(file):
    reader = csv.reader(file)
    cols = reader.next()

    def num(x):
        try:
            return float(x)
        except ValueError:
            return float('nan')

    conv = [str, num, num, num, num, num, num, num, num, str, num, num, num, num, num, num, num, num, num, num]
    pl = defaultdict(list)
    for row in reader:
        tmp = defaultdict(list)
        for h,v,c in zip(cols, row, conv):
            print c(v)
            pl[h].append(c(v))

    for k,v in pl.iteritems():
        pl[k] = array(v)

    print pl
    return pl

    #return [ dict(zip(cols, row)) for row in reader ]


def onpick(event):
    print event.artist

def onbuttonrelease(event):
    print event.inaxes
    try:
        i = subplots.index(event.inaxes)
        figs[0] = figs[i]
        #draw()
        fig.canvas.draw()
        #draw()
    except ValueError:
        print 'not found'
        pass

def ondraw(*args):
    global subplots
    print 'draw'
    fig = gcf()
    fig.clear()
    subplots = []
    ax = subplot2grid((4,4), (0,0), colspan=4, rowspan=3)
    subplots.append(ax)
    figs[0]('big')
    for i,f in enumerate(figs[1:]):
        ax = subplot2grid((4,4), (3,i))
        subplots.append(ax)
        f()

def fig1(which='small'):
    d = []

    print 'begin fig1'
    p = planets
    #d = reduce(add, zip(p['Pl. Semi-axis'], p['Pl. Mass'], ['go'] * len(p['Pl. Mass'])))
#   d = []
#   for p in planets:
#       print p
#       try:
#           d += map(float, [p['Pl. Period'], p['Pl. Mass']])
#           d.append('go')
#       except ValueError:
#           pass
            
    print p['Pl. Semi-axis']

    scatter(p['Pl. Semi-axis'], p['Pl. Mass']) #, hold=True)
#   if which=='big': 
#       grid(True, which='both')
#       xlabel(r'Orbital size (Astronomical Units)')
#       ylabel(r'Mass (Multiples of Earth)')
#       gca().xaxis.set_major_locator(LogLocator())
#       gca().yaxis.set_major_locator(LogLocator())
#       #gca().xaxis.set_major_formatter(FormatStrFormatter(r'$%0.6f$'))
#       #gca().yaxis.set_major_formatter(FormatStrFormatter(r'$%0.6f$'))
#       gca().xaxis.set_major_formatter(ScalarFormatter())
#       gca().yaxis.set_major_formatter(ScalarFormatter())
#       gca().xaxis.get_major_formatter().set_scientific(False)
#       gca().yaxis.get_major_formatter().set_scientific(False)
#       #gca().xaxis.tick_left()
#       #gca().yaxis.tick_bottom()
#   else:
#       gca().xaxis.set_major_locator(NullLocator())
#       gca().yaxis.set_major_locator(NullLocator())
    print 'end fig1'

def fig2(which='small'):
    d = []
    for p in planets:
        try:
            d += map(float, [p['Pl. Period'], p['Pl. Mass']])
            d.append('go')
        except ValueError:
            pass
            
    loglog(*d)
    if which=='big': 
        grid(True, which='both')
    else:
        gca().xaxis.set_major_locator(NullLocator())
        gca().yaxis.set_major_locator(NullLocator())


def fig3(which='small'):
    cir = Circle((0.9,0.5), radius=0.25,  fc='m', picker=True)
    gca().add_patch(cir)
    axis('scaled')

def fig4(which='small'):
    cir = Circle((0.9,0.5), radius=0.25,  fc='k', picker=True)
    gca().add_patch(cir)
    axis('scaled')


with open('exoplanetData.csv') as f:
    planets = readplanets(f)

#ion()

figs = [fig1, fig1]
#figs = [fig1, fig1, fig2, fig3, fig4]
subplots = []

fig = figure(facecolor='k', edgecolor='k')
#subplots_adjust(left=0, right=1, bottom=0.01, top=1)
fig.canvas.mpl_connect('pick_event', onpick)
fig.canvas.mpl_connect('button_release_event', onbuttonrelease)
fig.canvas.mpl_connect('draw_event', ondraw)

#manager = get_current_fig_manager()
#manager.window.after(100, ondraw)

show()

