import kiosk
from ex_planet_sim_info import *

import subprocess
import wxversion
wxversion.select('2.8')

import  wx
import  wx.lib.layoutf  as layoutf
import wx.lib.platebtn as platebtn
import wx.lib.buttons as gbuttons
import wx.media
#from wx.lib.colourutils import GetHighlightColour


lang = 'deutsch'

INFOTEXT='Explore the recent simulations of planet formation. Each\n' + \
         'picture to the left represents a different initial configuration.\n\n' + \
         'Click on a picture to see the configuration. Click the play \n' + \
         'button to see the simulation on the overhead monitor.'

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

              #INFOTEXT:
              #'Entdecken Sie die bisher gefundenen Exoplaneten. Diese Plots sind aktuell mit den neusten Entdeckungen!\n\n',
                #'Klicken Sie auf die Plots am unteren Rand des Bildschirms,\num die unterschiedlichen Relationen zu sehen. \r'
                #'Klicken Sie auf einen Punkt im Hauptplot, um Informationen ueber die einzelnen Planeten zu erhalten.',



            }
        T = t.get(s, None)
        if T is None:
            print "WARNING: no translation for '%s'" % s
            return s
        print s
        print '-->'
        print T
        return T
    else:
        assert 0

class MovieInfoText(wx.StaticText):
    def __init__( self, parent, str, size, **kwargs ):
        wx.StaticText.__init__( self, parent, label=str, size=(500,250), pos=(1350,350)) #, **kwargs )
        #font = wx.Font(size, wx.SWISS, wx.NORMAL, wx.NORMAL)
        #font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Krungthep')
        font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Courier New Bold')
        self.SetFont(font)
        self.SetForegroundColour('GREEN')
        #self.SetBackgroundColour('GREEN')
        self.update(None)

    def update(self, index):
        str = (
               _('Decay time:           %s\n') +
               _('Planetesimal Mass:    %s\n') +
               _('Formed Planets:    %s\n') +
               _('Mean eccentricity:    %s\n') +
               _('Mean semimajor axis:  %s\n'))

        f = lambda fmt,x,units: (fmt % x) + ' ' + units if not None else '%9s'%'Unknown'
        if index:
            self.SetLabel(str % (
                f('%6.2f', ALL_DATA[index][0], _('Myr')),
                f('%6.2f', ALL_DATA[index][2], _('Earth masses')),
                f('%6i', ALL_DATA[index][3], ''),
                f('%6.2f', ALL_DATA[index][7], 'AU'),
                f('%6.2f', ALL_DATA[index][8], 'AU')
                ))
        else:
            self.SetLabel('')

class HelpText(wx.StaticText):
    def __init__( self, parent, str, size, pos, **kwargs ):
        self.str = str
        wx.StaticText.__init__( self, parent, size=size, pos=pos) #, **kwargs )
        self.Wrap(-1)
        font = wx.Font(20, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="UniveLTLigUltCon")
        #font = wx.Font(22, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, faceName="Univers LT Std 57 Cn")
        self.SetFont(font)
        self.SetForegroundColour('WHITE')
        self.SetBackgroundColour('BLACK')
        self.draw(force=True)

    def select(self, index):
        pass

    def draw(self, force=False):
        self.SetLabel(_(self.str))

def mybox(p, x,y,w,h, c):
    j = wx.Panel(p, -1, style=wx.NO_BORDER, size=(w,1), pos=(x,y))
    k = wx.Panel(p, -1, style=wx.NO_BORDER, size=(w,1), pos=(x,y+h))
    l = wx.Panel(p, -1, style=wx.NO_BORDER, size=(1,h), pos=(x,y))
    m = wx.Panel(p, -1, style=wx.NO_BORDER, size=(1,h), pos=(x+w,y))
    j.SetBackgroundColour(c)
    k.SetBackgroundColour(c)
    l.SetBackgroundColour(c)
    m.SetBackgroundColour(c)
    return [j,k,l,m, w,h,c]

def move_mybox(b, x,y):
    j,k,l,m, w,h,c = b

    j.SetPosition([x,y])
    k.SetPosition([x,y+h])
    l.SetPosition([x,y])
    m.SetPosition([x+w,y])

class PCMovie(wx.Frame):

    def __init__(self, parent, id, **kwargs):
        wx.Frame.__init__(self, parent, id, **kwargs)

        panelStyle = wx.SIMPLE_BORDER
        panelStyle = wx.DOUBLE_BORDER
        panelStyle = wx.NO_BORDER

        p = wx.Panel(self, -1, style=wx.NO_BORDER)
        p.SetBackgroundColour('BLACK')
        self.p = p

        self.mc = wx.media.MediaCtrl(self, -1, style=wx.SIMPLE_BORDER, pos=(0,0), size=(1920,1080),szBackend=wx.media.MEDIABACKEND_QUICKTIME)

class PCFrame(wx.Frame):

    def __init__(self, parent, id, **kwargs):
        #wx.Frame.__init__(self, parent, id, title, size=size, pos=(0,0))
        wx.Frame.__init__(self, parent, id, **kwargs)
        #wx.Frame.__init__(self, parent, id, title, size=size, pos=(-350,-30))
        #self.Bind(wx.EVT_CLOSE, self.OnQuit)

        panelStyle = wx.SIMPLE_BORDER
        panelStyle = wx.DOUBLE_BORDER
        panelStyle = wx.NO_BORDER

        p = wx.Panel(self, -1, style=wx.NO_BORDER)
        p.SetBackgroundColour('BLACK')
        self.p = p

        #CHbmp   = wx.Image('Buttons/buttons_deutsch_1.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        CHbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_deutsch.png', wx.BITMAP_TYPE_PNG))
        CHOverbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_deutsch.png', wx.BITMAP_TYPE_PNG))
        ENbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_english.png', wx.BITMAP_TYPE_PNG))
        ENOverbmp   = wx.BitmapFromImage(wx.Image('Buttons/buttons_english.png', wx.BITMAP_TYPE_PNG))
        Helpbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))
        HelpOverbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))

        Playbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_start.png',    wx.BITMAP_TYPE_PNG))
        PlayOverbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_start.png',    wx.BITMAP_TYPE_PNG))

        bp = wx.Panel(p, -1, style=wx.NO_BORDER, size=(1920,1), pos=(0,1080-50))
        bp.SetBackgroundColour('#222222')

        CHbutton = gbuttons.GenBitmapButton(p, -1, CHbmp, style=wx.NO_BORDER,size=(120,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        #ENbutton   = wx.BitmapButton(p, -1, ENbmp,   style=wx.BU_EXACTFIT, size=(120,40))
        ENbutton = gbuttons.GenBitmapButton(p, -1, ENbmp, style=wx.NO_BORDER,size=(120,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        Helpbutton = gbuttons.GenBitmapButton(p, -1, Helpbmp, style=wx.NO_BORDER,size=(40,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        #Helpbutton = platebtn.PlateButton(p, -1, "", Helpbmp, style=platebtn.PB_STYLE_DEFAULT, size=(40,40))
        #Helpbutton = platebtn.PlateButton(p, -1, Helpbmp, style=wx.BU_EXACTFIT, size=(40,40))
        #Helpbutton.SetPressColor(wx.RED)
        #CHbutton.SetPressColor(wx.RED)
        CHbutton.SetBitmapSelected(CHOverbmp)
        ENbutton.SetBitmapSelected(ENOverbmp)
        Helpbutton.SetBitmapSelected(HelpOverbmp)

        CHbutton.SetPosition((1920-50-130-130,1080-45))
        ENbutton.SetPosition((1920-50-130,1080-45))
        Helpbutton.SetPosition((1920-50,1080-45))

        #CHbutton.SetPosition((1920-50-130-130,80-45))
        #ENbutton.SetPosition((1920-50-130,80-45))
        #Helpbutton.SetPosition((1920-50,80-45))

        self.SimIcon = None

        self.PlayBtn = gbuttons.GenBitmapButton(p, -1, Playbmp, style=wx.NO_BORDER,size=(40,40), pos=(1460,270))
        self.PlayBtn.Enable(False)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, self.PlayBtn)

        self.CHBtn = CHbutton
        self.ENBtn = ENbutton
        self.HelpBtn = Helpbutton

        self.Bind(wx.EVT_BUTTON, self.OnButton)
        #self.Bind(wx.EVT_TOGGLEBUTTON, self.OnToggle)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, ENbutton)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, Helpbutton)

        HelpText(p, INFOTEXT, (600,100), (1350,65))

        self.info_text = MovieInfoText(p, '', 20)
        self.info_text.SetBackgroundColour('BLACK')

        self.hilightbox = None
        self.sim_selected = None

        x,y = 100,20
        w,h = 125,125
        self.mbtns = []
        for i in range(8):
            for j in range(8):
                if [i,j] == [0,0]:
                    bmp = wx.BitmapFromImage(wx.Image('mov-%i-%i.png'% (i,i), wx.BITMAP_TYPE_PNG))
                #b = gbuttons.GenBitmapButton(p, -1, bmp, style=wx.NO_BORDER,size=(110,110), pos=(x+w*j, y+h*i)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
                #b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_DEFAULT, size=(110,110), pos=(x+w*j, y+h*i))
                #b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_SQUARE|platebtn.PB_STYLE_TOGGLE, size=(110,110), pos=(x+w*j, y+h*i))
                b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_SQUARE, size=(110,110), pos=(x+w*j, y+h*i))
                b.index = i * 8 + j
                b.SetBackgroundColour(wx.BLACK)
                b.SetPressColor(wx.RED)
                b.SetLabelColor(wx.RED)
                #self.Bind(wx.EVT_BUTTON, self.OnButton, b)
                self.mbtns.append([b,bmp])

    def OnButton(self, event):
        global lang
        obj = event.GetEventObject()
        if obj is self.CHBtn:
            print 'German!'
            if lang != 'deutsch': 
                lang = 'deutsch'
                #redraw()

        elif obj is self.ENBtn:
            print 'English!'
            if lang != 'english': 
                lang = 'english'
                #redraw()
        elif obj is self.HelpBtn:
            print 'Help!'

        elif obj is self.PlayBtn:
            r,c = divmod(self.sim_selected, 8)
            print r,c
            self.mc.Load('movie.mpg')
            self.mc.Play()
            pass

#       elif obj is self.PlayBtn:
#           r,c = divmod(self.sim_selected, 8)
#           print r,c
#           print >>mplayer.stdin, 'loadfile "movie.mpg"'
#           #print >>mplayer.stdin, 'loadfile "mf://mgen/m5/*png"'
#           #print >>mplayer.stdin, 'vo_fullscreen 1'
#           print >>mplayer.stdin, 'osd 0'
#           print >>mplayer.stdin, 'loop 1'
#           print >>mplayer.stdin, 'osd_show_text "hello" 0 10'
#           #self.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
#           self.SetFocus()
#           pass

        else:
            for b,bmp in self.mbtns:
                if b is obj:
                    if self.hilightbox is None:
                        self.hilightbox = mybox(self.p, 0,0,123,88, '#AAAAAA')
                    if self.SimIcon is None:
                        self.SimIcon = wx.StaticBitmap(self.p, -1, bmp, pos=(1350,250))

                    self.SimIcon.SetBitmap(bmp)

                    x,y = b.GetPositionTuple()
                    move_mybox(self.hilightbox, x,y)
                    b.Enable(False)
                    self.PlayBtn.Enable(True)
                    self.info_text.update(b.index)
                    self.sim_selected = b.index
                else:
                    b.Enable(True)

    def OnQuit(self, event):
        print 'Quit from keyboard disabled.'

class PCApp(wx.App):

    def OnInit(self):

        frame = PCFrame(None, -1, title=_('This should be hidden in Fullscreen'), size=(1920,1080), pos=(0,0), style=wx.FRAME_NO_TASKBAR)
        frame.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
        #frame.SetPosition((1920,0))
        frame.Show(True)
        self.frame = frame

        mframe = PCMovie(None, -1, title=_('This should be hidden in Fullscreen'), size=(1920,1080), pos=(1920,0), style=wx.FRAME_NO_TASKBAR)
        #frame.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
        #frame.SetPosition((2000,0))
        mframe.Show(True)

        frame.mc = mframe.mc
        frame.SetFocus()
        frame.CaptureMouse()
        self.SetTopWindow(frame)

        return True


if __name__ == '__main__':

#   mplayer = subprocess.Popen(['mplayer', 
#                               '-mf', 'type=png:fps=15', 
#                               '-vo', 'corevideo:device_id=1', 
#                               #'-input', 'nodefault-bindings:conf=/dev/null',
#                               '-quiet', '-slave', '-idle',
#                               '-loop', '0',
#                               '-fs',
#                               '-nomouseinput'
#                               ], stdin=subprocess.PIPE)
    #mplayer.stdout.close()


    app = PCApp(0)
    app.MainLoop()
