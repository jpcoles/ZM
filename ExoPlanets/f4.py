import kiosk
import matplotlib as mpl
from matplotlib import rc
from operator import add

import matplotlib.font_manager as fm
#print fm.get_fontconfig_fonts()

mpl.rcParams['toolbar'] = 'None'
#print filter(lambda x: x.startswith('font'), mpl.rcParams.keys())
#print filter(lambda x: x.startswith('/Library/Fonts/Univers'), fm.get_fontconfig_fonts().keys())
#rc('font',**{'name': 'Univers LT STd', 'size':20})
#rc('font',**{'family':'serif','serif':['Computer Modern Roman']})
#rc('text', usetex=True)
rc('font', size=20)
#rc('font', **{'family':'UniversLTStd-Cn'})
#rc('font', **{'family':'Univers LT Std 57 Cn', 'serif':'57 Cn', 'sans-serif':'57 Cn'})
#rc('font', **{'family':'UniversLTStd-Cn'})
#rc('font', **{'family':'Univers LT Std 57 Cn'})
rc('text', color='w')
rc('grid', color='w')
rc('axes', edgecolor='w')
rc('axes', facecolor='k')
rc('axes', labelcolor='w')
rc('axes', linewidth=2)
rc('ytick', color='w')
rc('xtick', color='w')
rc('xtick.major', pad=10)
rc('ytick.major', pad=10)
rc('text', dvipnghack=True)

lang = 'deutsch'

#_ = lambda x: x

INFOTEXT1='Explore the exoplanets that have been detected.\nThese plots are kept up to date with the latest discoveries! \n' + \
          'Click on a point in the main plot to show\ninformation about that planet.'

INFOTEXT2='Click on a diagram to the left\nto show in the upper panel.'

btable = { 'O': -4.3,
           'B': -3.00,
           'A': -0.15,
           'F': -0.01,
           'G': -0.10,
           'K': -0.24,
           'M': -2.5
         }

def _(s):
    if lang == 'english':
        return s
    elif lang == 'deutsch':
        t = {r'Orbital radius (Astronomical Units)': r'Umlaufbahn Radius (astronomische Einheit)',
             r'Mass (Multiples of Earth)'          : r'Masse (Mehrlinge von der Erde)',
             r'Orbital radius vs. Mass'            : r'Umlaufbahn Radius vs. Masse',
             r'Sunlight strength (Earth units)'    : r'Sonnenlicht Grosse (Erde Einheiten)',
             r'Gravity strength (Earth units)'     : r'Schwerkraft Grosse (Erde Einheiten)',
             r'Sunlight vs Gravity'                : r'Sonnenlicht vs. Schwerkraft',
             r'Reflex velocity (m/s)'              : r'Reflex Geschwindigkeit (m/s)',
             r'Mass (Multiples of Earth)'          : r'Masse (Mehrlinge von der Erde)',
             r'Reflex velocity vs. Mass'           : r'Reflex Geschwindigkeit vs. Masse',
              'Mass:           %s\n'               :  'Masse:               %s\n',
              'Radius:         %s\n'               :  'Radius:              %s\n',
              'Year:           %s\n'               :  'Jahr:                %s\n',
              'Orbit radius:   %s\n'               :  'Umlaufbahn Radius:   %s\n',

              'Earth masses'                       :  'Erdmassen',
              'Earth radii'                        :  'Erdradien',
              'Earth years'                        :  'Erdjahre',

             r'Mercury'                            : r'Merkur',
             r'Venus'                              : r'Venus',
             r'Earth'                              : r'Erde',
             r'Mars'                               : r'Mars',
             r'Jupiter'                            : r'Jupiter',
             r'Saturn'                             : r'Saturn',
             r'Uranus'                             : r'Uranus',
             r'Neptune'                            : r'Neptun',

              INFOTEXT1:
               'Erkunden Sie die nachgewiesenen Exoplaneten.\nDiese Aufzeichnungen werden laufend mit den neusten Entdeckungen aktualisiert.\n'
               'Klicken Sie auf einen Punkt im Hauptplot, um\nInformationen ueber die einzelnen Planeten zu erhalten.',
              #'Entdecken Sie die bisher gefundenen Exoplaneten. Diese Plots sind aktuell mit den neusten Entdeckungen!\n\n',
                #'Klicken Sie auf die Plots am unteren Rand des Bildschirms,\num die unterschiedlichen Relationen zu sehen. \r'
              INFOTEXT2:
               'Klicken Sie in eines der Diagramme links\num oben die Vergroesserung anzuzeigen.'
            }
        T = t.get(s, None)
        if T is None:
            print "WARNING: no translation for '%s'" % s
            return s
#       print s
#       print '-->'
#       print T
        return T
    else:
        assert 0

import wxversion
wxversion.select('2.8')

import  wx
import  wx.lib.layoutf  as layoutf
import wx.lib.platebtn as platebtn
import wx.lib.buttons as gbuttons
from wx.lib.fancytext import RenderToBitmap
from wxPlotPanel import PlotPanel
try:
    from agw import advancedsplash as AS
except ImportError: # if it's not there locally, try the wxPython lib.
    import wx.lib.agw.advancedsplash as AS


from pylab import figure,show,plot, Circle, gca, axis, subplot, \
                  draw, gcf, axes, subplots_adjust, loglog, grid,\
                  xlabel, ylabel, ticklabel_format, tick_params, \
                  xticks, yticks, NullLocator, ScalarFormatter, \
                  LogLocator, FormatStrFormatter,scatter

from pylab import ion, ioff, get_current_fig_manager, draw

from matplotlib.pyplot import subplot2grid

from numpy import arange,sin, array, isfinite, logical_and, nan, exp, log, sqrt, isnan, abs, log10
import csv

from collections import defaultdict

class EPFormatter(ScalarFormatter):
    def __init__(self, everyother=False):
        self.everyother = everyother
        ScalarFormatter.__init__(self)

    def __call__(self, n, index):
        if self.everyother and (index%2)==0: return ''

        if n >= 1 or n == 0: 
            return r'$%i$' % n
        else:
            return r'$%.*f$'%(int(abs(log10(n))), n)
            #return r'$%s$' % str(n)
        

def readplanets(file):
    reader = csv.reader(file)
    cols = reader.next()

    def num(x):
        try:
            return float(x)
        except ValueError:
            return nan

    conv = [str, num, num, num, num, num, num, num, num, str, num, num, num, num, num, num, num, num, num, num]
    pl = defaultdict(list)
    for row in reader:
        for h,v,c in zip(cols, row, conv):
            pl[h].append(c(v))

    colors = ['blue'] * len(pl['Planet Name'])

    cols = ['Planet Name', 'Pl. Mass', 'Pl. Radius', 'Pl. Period', 'Pl. Semi-axis', 'St. Dist', 'St. Mag. V.', 'St. Mass']
    sol = [ ['Mercury',   0.055,  0.38,   0.24,  0.39, 10, 4.8, 1],
            ['Venus',     0.820,  0.95,   0.62,  0.72, 10, 4.8, 1],
            ['Earth',     1.000,  1.00,   1.00,  1.00, 10, 4.8, 1],
            ['Mars',      0.110,  0.53,   1.88,  1.52, 10, 4.8, 1],
            ['Jupiter', 317.830, 11.00,  11.90,  5.20, 10, 4.8, 1],
            ['Saturn',   95.000,  9.00,  29.50,  9.60, 10, 4.8, 1],
            ['Uranus',   14.500,  3.90,  84.00, 19.00, 10, 4.8, 1],
            ['Neptune',  17.000,  3.80, 165.00, 30.00, 10, 4.8, 1] ]

    colors += ['red'] * len(sol)

    sol = [ dict(zip(cols, s)) for s in sol ]

    for k,v in pl.iteritems():
        pl[k].extend( [ s.get(k, nan) for s in sol ] )

    for k,v in pl.iteritems():
        pl[k] = array(v)

    # Set the stellar mass for all pulsars to 1.4
    w = array(map(lambda x: x[:3] == 'PSR', pl['Planet Name']))
    pl['St. Mass'][w] = 1.4


    pl['Pl. Mass']  [:-8] *= 317.83 # Convert to Earth masses
    pl['Pl. Period'][:-8] /= 365.25 # Convert to years
    pl['Pl. Radius'][:-8] *= 11.209 # Convert to Earth radii

    pl['Pl. Grav']  = pl['Pl. Mass'] / pl['Pl. Radius']**2
    w = isnan(pl['Pl. Radius'])
    pl['Pl. Grav'][w] = pl['Pl. Mass'][w]**0.3 * (0.5)**(0.66)

    f = lambda x: btable.get(x[0], 0) if len(x) else 0
    print pl['St. Spec. Type']
    bolcorr = array([ f(x) for x in pl['St. Spec. Type'] ])
    print bolcorr

    pl['Pl. Sol']   = 10 ** (0.4 * (4.8-(pl['St. Mag. V.'] + bolcorr))) * (pl['St. Dist']/(10*pl['Pl. Semi-axis']))**2
    pl['Pl. Vref']  = 9e-2 * 319 * pl['Pl. Mass'] / 317.83 / sqrt(pl['St. Mass']*pl['Pl. Semi-axis'])

    #print pl['St. Mag. V.'][-8:]
    #print pl['Pl. Sol'][-8:]

#   for i,n in enumerate(pl['Planet Name']):
#       if n.startswith('Gl 581'):
#           colors[i] = 'green'


    #print pl
    return pl, array(colors)

    #return [ dict(zip(cols, row)) for row in reader ]


def StaticText(parent, str, fontsize=24, **kwargs):
    font = wx.Font(fontsize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="Univers LT Std 57 Cn")
    #font = wx.Font(fontsize, wx.SWISS, wx.NORMAL, wx.NORMAL)
    t = wx.StaticText(parent, -1, str, **kwargs); 
    t.SetFont(font)
    t.SetForegroundColour('WHITE')
    return t

def make_section(parent, orient=wx.HORIZONTAL, border=wx.ALL):

    w = wx.Panel(parent, -1, style=wx.NO_BORDER); w.SetBackgroundColour('#550000')
    #p = wx.Panel(w,      -1, style=wx.NO_BORDER); p.SetBackgroundColour('BLACK')

    #box0 = wx.BoxSizer(wx.VERTICAL)
    #box1 = wx.BoxSizer(orient)

    #box0.Add(p, 1, wx.EXPAND|border, 0)

    #w.SetSizer(box0); w.SetAutoLayout(True)
    #p.SetSizerAndFit(box1)#; p.SetAutoLayout(True)

    return w,w, None

def make_special_section(parent, panelClass, border, sp, **kwargs):

    #return make_section(parent, border=border)[:2]

    print "SADFS"
    #w = wx.Panel(parent, -1, style=wx.NO_BORDER, size=sp[0], pos=sp[1]); 
    #w.SetBackgroundColour('PURPLE')
    #w.SetBackgroundColour('#555555')
    #w.SetBackgroundColour('#111111')
    #parent = w

    #p = panelClass(parent, size=sp[0], **kwargs)
    p = panelClass(parent, size=sp[0], pos=sp[1], **kwargs)
    container = p
    #p.SetBackgroundColour('GREEN')

#   box0 = wx.BoxSizer(wx.VERTICAL)
#   box0.Add(p, 1, wx.EXPAND|border, 0)
#   #p.SetSizerAndFit(box0)
#   w.SetSizer(box0) #AndFit(box0) ; 
#   w.SetAutoLayout(True)
#   container = w
    #container = parent

    return container,p

def select(index):
    global selected
    selected = index
    for p in plot_diagrams:
        p.select(index)

def redraw():
    print 'redraw!'
    for p in plot_diagrams:
        p.select(selected)
        p.draw() #force=True)
        #p.Update()
        #p.Refresh()
        
    

class HelpText(wx.StaticText):
    def __init__( self, parent, str, fontsize, size, pos, **kwargs ):
        self.str = str
        #wx.StaticText.__init__( self, parent, pos=pos) #, **kwargs )
        wx.StaticText.__init__( self, parent, size=size, pos=pos) #, **kwargs )
        #self.Wrap(False)
        #self.Wrap(size[0])
        font = wx.Font(fontsize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="UniveLTLigUltCon")
        #font = wx.Font(22, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="Univers LT Std 57 Cn")
        self.SetFont(font)
        self.SetForegroundColour('WHITE')
        self.SetBackgroundColour('BLACK')
        plot_diagrams.append(self)
        self.draw(force=True)
        #print self.GetSize()

    def select(self, index):
        pass

#   def draw(self, force=False):
#       pass

    def draw(self, force=False):
        self.SetLabel(_(self.str))

class AxisLabel:
    def __init__( self, parent, str, axis, fontsize, size, pos, **kwargs ):
        self.str = str
        self.parent = parent
        self.pos = pos

        self.cache = {}
        self.fontsize = fontsize
        self.axis = axis
        self.size = size
        self.draw(force=True)

    def select(self, index):
        pass

    def draw(self, force=False):
        s = _(self.str)
        if not self.cache.has_key(s):
            font = wx.Font(self.fontsize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="UniveLTLigUltCon")

            b = wx.EmptyBitmap(*self.size)
            dc = wx.MemoryDC(b)
            dc.SetFont(font)
            dc.SetTextForeground('white')
            dc.SetTextBackground('black')
            dc.DrawText(s, 0,0)
            pos = self.pos
            if self.axis == 'y':
                pos = (self.pos[0], self.pos[1]-self.size[0]-font.GetPixelSize()[0])
                b = b.ConvertToImage().Rotate90(clockwise=False).ConvertToBitmap()
            b = wx.StaticBitmap(self.parent, -1, b, pos=pos)
            b.Raise()
            self.cache[s] = b

        self.Hide()

        #self.cache[s].Show()

    def Hide(self):
        print 'hiding', _(self.str)
        for k,v in self.cache.iteritems():
            v.Hide()

    def Show(self):
        s = _(self.str)
        print 'showing', s
        self.draw()
        self.cache[s].Show()

#class PlotInfoText(wx.StaticText):
#    def __init__( self, parent, str, size, **kwargs ):
#        wx.StaticText.__init__( self, parent, size=(600,200), pos=(1350,65)) #, **kwargs )
#        self.Wrap(-1)
#        font = wx.Font(size, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="Univers LT Std 57 Cn")
#        self.SetFont(font)
#        self.SetForegroundColour('WHITE')
#        self.SetBackgroundColour('BLACK')
#        plot_diagrams.append(self)
#        self.draw(force=True)
#
#    def select(self, index):
#        pass
#
#    def draw(self, force=False):
#        self.SetLabel()

class PlanetInfoText(wx.StaticText):
    def __init__( self, parent, str, size, **kwargs ):
        wx.StaticText.__init__( self, parent, label=str, size=(500,200), pos=(1350,300)) #, **kwargs )
        #font = wx.Font(size, wx.SWISS, wx.NORMAL, wx.NORMAL)
        #font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Krungthep')
        font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Courier New Bold')
        self.SetFont(font)
        self.SetForegroundColour('GREEN')
        #self.SetBackgroundColour('GREEN')
        plot_diagrams.append(self)

    def select(self, index):
        str = ('%s\n\n' +
               _('Mass:           %s\n') +
               _('Radius:         %s\n') +
               _('Year:           %s\n') +
               _('Orbit radius:   %s\n'))

        f = lambda fmt,x,units: (fmt % x) + ' ' + units if not isnan(x) else '%9s'%'Unknown'
        if index:
            self.SetLabel(str % (
                _(planets['Planet Name'][index]),
                f('%6.2f', planets['Pl. Mass'][index], _('Earth masses')),
                f('%6.2f', planets['Pl. Radius'][index], _('Earth radii')),
                f('%6.2f', planets['Pl. Period'][index], _('Earth years')),
                f('%6.2f', planets['Pl. Semi-axis'][index], 'AU')
                ))
        else:
            self.SetLabel('')

    def draw(self, force=False):
        pass

class PlotDiagram(PlotPanel):
    def __init__( self, parent, issmall, labels, **kwargs ):
        PlotPanel.__init__( self, parent, **kwargs )


        self.parent = parent
        self.subplot = None
        self.small_panel_index = None
        self.issmall = issmall
        self.artist = None
        self.selected = None

        if self.issmall: 
            self.figure.canvas.mpl_connect('button_release_event', self)
        else:
            self.figure.canvas.mpl_connect('pick_event', self)

#       if self.issmall:
#           self.smallxlabel = labels[0]
#       else:
#           self.xlabel, self.ylabel = labels

        self.labels = labels

        plot_diagrams.append(self)

        #self.subplot = self.figure.add_subplot( 111 )

    def set_small_panel_index(self, index):
        self.small_panel_index = index

    def select(self, index):
        if self.artist is not None:
            n = self.active.size
            if self.issmall:
                lw = [1] * n
                r = [40] * n
            else:
                lw = [1] * n
                r = [80] * n
            #clrs = colors[self.active]
            clrs = ['black'] * n

            print 'index/selected', index, self.selected
            if index is not None:
                self.selected = index
#               index = (self.active==index).nonzero()[0]
#               print index, self.selected
#               if index:
#                   clrs[index] = 'orange'
#                   r[index] = 200
#                   #lw[index] = 0
#               else:
#                   self.selected = None

            self.artist.set_edgecolors(clrs)
            self.artist._sizes = r
            #self.artist.set_linewidths(lw)
            #print self.figure.canvas.GetSize()
            #self.figure.canvas.draw()
            #print self.figure.canvas.GetSize()

#   def draw( self, force=False ):
#       assert False, 'draw() must be overridden'
    
    def draw( self, force=False ):
        #print "DRAW!", self.GetSizeTuple()
        if self.subplot is None:
            #print 'drawing!'
            self.subplot = self.figure.add_subplot( 111 )
            force = True

        if force: 
            self.artist = self.plot()
            #self.figure.canvas.draw()
#           if self.artist is None:
#               self.artist = self.plot()
#               self.figure.canvas.draw()
#           else:
#               self.artist = self.plot()
#               self.figure.draw_artist(self.artist)
            #self.figure.canvas.draw_artist(self.artist)
            #self.figure.canvas.draw_artist(self.artist)
            #self.select(self.selected)

#       if self.IsShown():
#           print 'showing axis labels'
#           if not self.issmall:
#               #print self.xlabel.str
#               self.xlabel.Show()
#               self.ylabel.Show()
#           else:
#               self.smallxlabel.Show()
#       else:
#           print 'hiding axis labels'
#           if not self.issmall:
#               #print self.xlabel.str
#               self.xlabel.Hide()
#               self.ylabel.Hide()
#           else:
#               self.smallxlabel.Hide()

    def __call__(self, event):

        if event.name == 'button_release_event':
            app.frame.set_main_plot(self.small_panel_index)
            #print 'here'
            #select(self.selected)
            return True
            
        if event.name == 'pick_event' and event.artist == self.artist :
            select(self.active[event.ind[0]])
            return True

    def plot(self):
        assert False, 'plot() must be overridden'

    def Show(self, *args):
        PlotPanel.Show(self, *args)
        for x in self.labels: x.Show()

    def Hide(self, *args):
        PlotPanel.Hide(self, *args)
        for x in self.labels: x.Hide()


class Fig1(PlotDiagram):
    def __init__( self, parent, issmall, pos=None, size=None, color=None ):
        PlotDiagram.__init__( self, parent, issmall, [], pos=pos, size=size, color=color )
        if not issmall:
            self.labels = [AxisLabel(parent, 'Orbital radius (Astronomical Units)', 'x', 20, (600,240), (pos[0]+170,pos[1] + size[1] - 35)), 
                      AxisLabel(parent, 'Mass (Multiples of Earth)', 'y', 20, (600,240), (pos[0]+55,pos[1]+size[1]-55)) ]
        else:
            self.labels = [ AxisLabel(parent, 'Orbital radius vs. Mass', 'x', 20, (600,240), (pos[0]+65,pos[1] + size[1] - 28)) ]


    def plot(self):
        ax = self.subplot
            
        if self.artist is None:
            xs = planets['Pl. Semi-axis']
            ys = planets['Pl. Mass']
            self.active = w = logical_and(isfinite(xs), isfinite(ys)).nonzero()[0]
            xs,ys,clrs = xs[w], ys[w], colors[w]

            ax.set_xscale('log')
            ax.set_yscale('log')

            if not self.issmall: 
                artist = ax.scatter(xs, ys, s=80, c=clrs, lw=1, picker=80., zorder=1000)
                ax.grid(True, which='both', ls='-', c='#222222')
                ax.xaxis.set_major_formatter(EPFormatter())
                ax.yaxis.set_major_formatter(EPFormatter())
            else:
                artist = ax.scatter(xs, ys, s=40, c=clrs, lw=1, picker=80.)
                ax.xaxis.set_major_locator(NullLocator())
                ax.yaxis.set_major_locator(NullLocator())

            #ax.axis('scaled')
            ax.set_xlim(1e-3, 1e3)
            ax.set_ylim(1e-2, 1e4)

            ax.figure.canvas.draw()
        else:
            artist = self.artist

        return artist


#   def draw(self, *args, **kwargs):
#       PlotDiagram.draw(self, *args, **kwargs)
#       if self.IsShown():
#           if not self.issmall:
#               print 'showing axis labels'
#               self.xlabel.Show()
#               self.ylabel.Show()
#           else:
#               self.smallxlabel.Show()
#       else:
#           print 'hiding axis labels'
#           if not self.issmall:
#               self.xlabel.Hide()
#               self.ylabel.Hide()
#           else:
#               self.smallxlabel.Hide()

class Fig2(PlotDiagram):
    def __init__( self, parent, issmall, pos=None, size=None, color=None ):
        PlotDiagram.__init__( self, parent, issmall, [], pos=pos, size=size, color=color )
        if not issmall:
            self.labels = [ AxisLabel(parent, 'Sunlight strength (Earth units)', 'x', 20, (600,240), (pos[0]+170,pos[1] + size[1] - 35)), 
                       AxisLabel(parent, 'Gravity strength (Earth units)', 'y', 20, (600,240), (pos[0]+55,pos[1]+size[1]-55)) ]
        else:
            self.labels = [ AxisLabel(parent, 'Sunlight vs Gravity', 'x', 20, (600,240), (pos[0]+65,pos[1] + size[1] - 28)) ]

    def plot(self):
        ax = self.subplot

        xs = planets['Pl. Sol']
        ys = planets['Pl. Grav']
        self.active = w = logical_and(isfinite(xs), isfinite(ys)).nonzero()[0]
        xs,ys,clrs = xs[w], ys[w], colors[w]


        ax.set_xscale('log')
        ax.set_yscale('log')

        if not self.issmall: 
            artist = ax.scatter(xs, ys, s=80, c=clrs, lw=1, picker=80., zorder=1000)
            ax.grid(True, which='both', ls='-', c='#222222')
            ax.axvspan(1./4,1./0.56, facecolor='#111111', zorder=-1000)
            #ax.set_xlabel(_(r'Sunlight strength (Earth units)'))
            #ax.set_ylabel(_(r'Gravity strength (Earth units)'))
            #ax.xaxis.set_major_formatter(FormatStrFormatter(r'$%0.2f$'))
            #ax.yaxis.set_major_formatter(FormatStrFormatter(r'$%0.2f$'))
            ax.xaxis.set_major_formatter(EPFormatter(True))
            ax.yaxis.set_major_formatter(EPFormatter())
        else:
            artist = ax.scatter(xs, ys, s=40, c=clrs, lw=1, picker=80.)
            #ax.set_xlabel(_(r'Sunlight vs Gravity'))
            ax.xaxis.set_major_locator(NullLocator())
            ax.yaxis.set_major_locator(NullLocator())

        #ax.axis('scaled')
        ax.set_xlim(1e-9, 2e4)
        ax.set_ylim(1e-1, 1e2)

        return artist

class Fig3(PlotDiagram):
    def __init__( self, parent, issmall, pos=None, size=None, color=None ):
        PlotDiagram.__init__( self, parent, issmall, [], pos=pos, size=size, color=color )
        if not issmall:
            self.labels = [ AxisLabel(parent, 'Reflex velocity (m/s)', 'x', 20, (600,240), (pos[0]+170,pos[1] + size[1] - 35)), 
                       AxisLabel(parent, 'Mass (Multiples of Earth)', 'y', 20, (600,240), (pos[0]+55,pos[1]+size[1]-55)) ]
        else:
            self.labels = [ AxisLabel(parent, 'Reflex velocity vs. Mass', 'x', 20, (600,240), (pos[0]+65,pos[1] + size[1] - 28)) ]

    def plot(self):
        ax = self.subplot

        xs = planets['Pl. Vref']
        ys = planets['Pl. Mass']
        self.active = w = logical_and(isfinite(xs), isfinite(ys)).nonzero()[0]
        xs,ys,clrs = xs[w], ys[w], colors[w]

        ax.set_xscale('log')
        ax.set_yscale('log')

        if not self.issmall: 
            artist = ax.scatter(xs, ys, s=80, c=clrs, lw=1, picker=80., zorder=1000)
            ax.grid(True, which='both', ls='-', c='#222222')
            #ax.set_xlabel(_(r'Reflex velocity (m/s)'))
            #ax.set_ylabel(_(r'Mass (Multiples of Earth)'))
            ax.xaxis.set_major_formatter(EPFormatter())
            ax.yaxis.set_major_formatter(EPFormatter())
        else:
            artist = ax.scatter(xs, ys, s=40, c=clrs, lw=1, picker=80.)
            #ax.set_xlabel(_(r'Reflex velocity vs. Mass'))
            ax.xaxis.set_major_locator(NullLocator())
            ax.yaxis.set_major_locator(NullLocator())

        #ax.axis('scaled')
        ax.set_xlim(1e-3, 1e4)
        ax.set_ylim(1e-2, 1e4)

        return artist

class EPFrame(wx.Frame):

    def __init__(self, parent, id, title, size):
        #wx.Frame.__init__(self, parent, id, title, size=size, pos=(-400,0))
        wx.Frame.__init__(self, parent, id, title, size=size, pos=(0,0), style=wx.FRAME_NO_TASKBAR)
        #self.Bind(wx.EVT_CLOSE, self.OnQuit)

        fw, fh = size

        panelStyle = wx.SIMPLE_BORDER
        panelStyle = wx.DOUBLE_BORDER
        panelStyle = wx.NO_BORDER

        p = wx.Panel(self, -1, style=wx.NO_BORDER)
        p.SetBackgroundColour('BLACK')
        #p.SetBackgroundColour('RED')

        wh = 1350,701
        o = 0,0
        bigsections = []
        for f in figs:
            s = make_special_section(p, lambda p, **kwargs: f(p,False,**kwargs), wx.SOUTH, 
                                     [wh, o], color=(0,0,0))
            bigsections.append(s)

        sections = []
        wh = 500,320
        offs = 0
        o = 0,691
        for i,f in enumerate(figs):
            s = make_special_section(p, lambda p, **kwargs: f(p,True, **kwargs), wx.EAST, 
                                     [wh, [o[0]+i*(wh[0]+offs),o[1]]], color=(0,0,0))
            s[1].set_small_panel_index(i)
            s[1].draw()
            sections.append(s)

        #CHbmp   = wx.Image('Buttons/buttons_deutsch_1.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        CHbmp       = wx.BitmapFromImage(wx.Image('Buttons/buttons_deutsch.png', wx.BITMAP_TYPE_PNG))
        CHOverbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_deutsch.png', wx.BITMAP_TYPE_PNG))
        ENbmp       = wx.BitmapFromImage(wx.Image('Buttons/buttons_english.png', wx.BITMAP_TYPE_PNG))
        ENOverbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_english.png', wx.BITMAP_TYPE_PNG))
        Helpbmp     = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))
        HelpOverbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))

        self.InfoDEbmp = wx.BitmapFromImage(wx.Image('_info_d_Extrasolarep_1920x1080.png',    wx.BITMAP_TYPE_PNG))
        self.InfoENbmp = wx.BitmapFromImage(wx.Image('_info_e_Extrasolarep_1920x1080.png',    wx.BITMAP_TYPE_PNG))
        self.infoFrame = None

        w,pnl,box = secCtrl = make_section(p, wx.HORIZONTAL, wx.NORTH)

        print '*' * 80
        print CHbmp.GetWidth(), CHbmp.GetHeight()

        bp = wx.Panel(p, -1, style=wx.NO_BORDER, size=(fw,1), pos=(0,fh-6-40-6-1))
        bp.SetBackgroundColour('#222222')

        CHbutton   = gbuttons.GenBitmapButton(p, -1, CHbmp,   style=wx.NO_BORDER,size=(120,40))
        ENbutton   = gbuttons.GenBitmapButton(p, -1, ENbmp,   style=wx.NO_BORDER,size=(120,40))
        Helpbutton = gbuttons.GenBitmapButton(p, -1, Helpbmp, style=wx.NO_BORDER,size=(40,40))
        CHbutton.SetBitmapSelected(CHOverbmp)
        ENbutton.SetBitmapSelected(ENOverbmp)
        Helpbutton.SetBitmapSelected(HelpOverbmp)
        print CHbutton.GetSize()

        #Helpbutton.SetBitmapSelected(HelpOverbmp)

        CHbutton.SetPosition((fw-15-40-15-120-15-120,fh-40-6))
        ENbutton.SetPosition((fw-15-40-15-120,fh-40-6))
        Helpbutton.SetPosition((fw-15-40,fh-40-6))

#       CHbutton.SetPosition((fw/1.5-15-40-15-120-15-120,0))
#       ENbutton.SetPosition((fw/1.5-15-40-15-120,0))
#       Helpbutton.SetPosition((fw/1.5-15-40,0))

        #CHbutton.SetPosition((fw-50-130-130,80-45))
        #ENbutton.SetPosition((fw-50-130,80-45))
        #Helpbutton.SetPosition((fw-50,80-45))

        self.CHbtn = CHbutton
        self.ENbtn = ENbutton
        self.Helpbtn = Helpbutton

        self.Bind(wx.EVT_BUTTON, self.OnButton, CHbutton)
        self.Bind(wx.EVT_BUTTON, self.OnButton, ENbutton)
        self.Bind(wx.EVT_BUTTON, self.OnButton, Helpbutton)
        #self.Bind(wx.EVT_PAINT, self.OnPaint)

        self.misctext = []
        self.misctext.append(HelpText(p, INFOTEXT1, 18, (600,240), (1350,65)))
        self.misctext.append(HelpText(p, INFOTEXT2, 18, (600,100), (1500,765)))

        t = PlanetInfoText(p, '', 20)
        t.SetBackgroundColour('BLACK')

        self.misctext.append(t)

        for s in sections:
            s[0].Show()
            s[1].draw()

        for s in bigsections:
            s[0].Hide()
            s[1].draw()

        #panelSL.draw()

        self.panel = p
        self.bigsections = bigsections

        self.current_main = 0
        #self.set_main_plot(0)
        bigsections[self.current_main][0].Show()
        bigsections[self.current_main][1].draw()

        self.show_info = False
        self.bitmap = None

        self.InfoDE = wx.StaticBitmap(p, -1, self.InfoDEbmp, (fw-self.InfoDEbmp.GetWidth(), 0))
        self.InfoEN = wx.StaticBitmap(p, -1, self.InfoENbmp, (fw-self.InfoENbmp.GetWidth(), 0))
        self.InfoDE.Raise()
        self.InfoEN.Raise()

        self.InfoDE.Hide()
        self.InfoEN.Hide()

    def set_main_plot(self, which):
        if which != self.current_main:
            old = self.bigsections[self.current_main]
            new = self.bigsections[which]

            new[0].Show()
            old[0].Hide()
            old[1].draw()
            #self.box0.Replace(0, self.bigsections[which][0])
            #self.box0.Replace(old[0], new[0])
            self.current_main = which
            new[1].draw()

            #new[1].select(None)
            print 'Show!'
            #self.box0.Layout()
            #self.panel.Layout()

#   def OnPaint(self, evt):
#       if self.show_info:
#           print 'show info'
#           pdc = wx.PaintDC(self)
#           try:
#               dc = wx.GCDC(pdc)
#           except:
#               dc = pdc
#           print self.bitmap
#           dc.DrawBitmap(self.bitmap, 1024, 0, False)

    def OnButton(self, event):
        global lang
        doredraw = False
        obj = event.GetEventObject()
        if obj is self.CHbtn:
            print 'German!'
            if lang != 'deutsch': 
                lang = 'deutsch'
                doredraw = True

        if obj is self.ENbtn:
            print 'English!'
            if lang != 'english': 
                lang = 'english'
                doredraw = True

        if doredraw:
            redraw()

        if obj is self.Helpbtn or (self.show_info and doredraw):
            if self.show_info and not doredraw:
                self.show_info = False
                self.InfoDE.Hide()
                self.InfoEN.Hide()
                for t in self.misctext:
                    t.Show()
            else:
                self.show_info = True
                if lang == 'english':
                    self.InfoDE.Hide()
                    self.InfoEN.Show()
                if lang == 'deutsch':
                    self.InfoDE.Show()
                    self.InfoEN.Hide()
                for t in self.misctext:
                    t.Hide()


    def OnInfoClose(self, event):
        print 'info close'
        if self.infoFrame is not None:
            print 'info close 2'
            self.infoFrame.Destroy()
            self.infoFrame = None

    def OnQuit(self, event):
        print 'Quit from keyboard disabled.'

class EPApp(wx.App):

    def OnInit(self):
        frame = EPFrame(None, -1, _('This should be hidden in Fullscreen'), size=(1920,1080))
        print "!@#!@#!@#!@#"
        #frame.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
        frame.Show(True)
        self.SetTopWindow(frame)
        self.frame = frame
        return True

if __name__ == '__main__':

    with open('exoplanetData.csv') as f:
        planets,colors = readplanets(f)

    plot_diagrams = []

    figs = [Fig1, Fig2, Fig3]
    #figs = [fig1, fig1, fig2, fig3, fig4]
    subplots = []

    #select(planets['Planet Name'].size - 8 + 3)
    selected = None

    #fig = figure(facecolor='k', edgecolor='k')
    #subplots_adjust(left=0, right=1, bottom=0.01, top=1)
    #fig.canvas.mpl_connect('pick_event', onpick)
    #fig.canvas.mpl_connect('button_release_event', onbuttonrelease)
    #fig.canvas.mpl_connect('draw_event', ondraw)

    app = EPApp(0)
    app.MainLoop()
