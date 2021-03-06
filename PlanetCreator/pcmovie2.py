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

INFOTEXT='Explore the recent planet formation simulations. Each' + \
         'picture\nto the left represents a different initial configuration.\n\n' + \
         'Click on a picture to see the configuration.\nClick the play' + \
         'button to see the simulation on the overhead monitor.'

def _(s):
    if lang == 'english':
        return s
    elif lang == 'deutsch':
        t = {'Planetesimal Mass:    %s\n': 'Planetoiden Masse:          %s\n',
             'Formed Planets:       %s\n': 'Geformte Planeten:          %s\n',
             'Average orbit radius: %s\n': 'Mittlerer Umlaufbahnradius: %s\n',
             'Average ellipticity:  %s\n': 'Mittlere Elliptizitaet:     %s\n',
             'Gas loss time:        %s\n': 'Gas Abstosszeit:            %s\n',

             'Earth masses': 'Erdmasse',
             'Unknown': 'Unbekannt',
             INFOTEXT: 'Erkunden Sie die aktuellen Planetentstehungs Simulationen.\n' + \
                       'Jedes Bild auf der linken Seite entspricht unterschiedlichen\n' + \
                       'Anfangsbedingungen.\n\nKlicken Sie auf das Bild um die Werte' + \
                       'zu sehen.\nKlicken Sie auf den Wiedergabeknopf um die\n' + \
                       'Simulation auf dem oberen Bildschirm zu sehen.'
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
        wx.StaticText.__init__( self, parent, label=str, size=(600,250), pos=(1150,350)) #, **kwargs )
        #font = wx.Font(size, wx.SWISS, wx.NORMAL, wx.NORMAL)
        #font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Krungthep')
        font = wx.Font(size, wx.NORMAL, wx.SYS_SYSTEM_FIXED_FONT, wx.NORMAL, face='Courier New Bold')
        self.SetFont(font)
        self.SetForegroundColour('GREEN')
        #self.SetBackgroundColour('GREEN')
        self.update(None)

    def update(self, index):
        str = (
               _('Planetesimal Mass:    %s\n') +
               _('Formed Planets:       %s\n') +
               _('Average orbit radius: %s\n') +
               _('Average ellipticity:  %s\n') +
               _('Gas loss time:        %s\n'))

        f = lambda fmt,x,units: (fmt % x) + ' ' + units if not None else '%9s'% _('Unknown')
        if index is not None:
            self.SetLabel(str % (
                f('%6.2f', ALL_DATA[index][2], _('Earth masses')),
                f('%6i', ALL_DATA[index][3], ''),
                f('%6.2f', ALL_DATA[index][7], 'AU'),
                f('%6.2f', ALL_DATA[index][8], ''),
                f('%6.2f', ALL_DATA[index][0], _('Myr'))
                ))
        else:
            self.SetLabel('')
        self.index = index

    def draw(self):
        self.update(self.index)

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

class TempMediaCtrl:
    def __init__(self):
        self.fname = None

    def Load(self, fname):
        self.fname = fname
        print 'Loading', self.fname

    def Play(self):
        print 'Playing', self.fname

class PCMovie(wx.Frame):

    def __init__(self, parent, id, **kwargs):
        wx.Frame.__init__(self, parent, id, **kwargs)

        panelStyle = wx.SIMPLE_BORDER
        panelStyle = wx.DOUBLE_BORDER
        panelStyle = wx.NO_BORDER

        p = wx.Panel(self, -1, style=wx.NO_BORDER)
        p.SetBackgroundColour('BLACK')
        self.p = p

        try:
            self.mc = wx.media.MediaCtrl(self, -1, style=wx.SIMPLE_BORDER, pos=(0,0), size=(1920,1102),szBackend=wx.media.MEDIABACKEND_QUICKTIME)
            self.mc.SetBackgroundColour('BLACK')
        except NotImplementedError:
            self.mc = TempMediaCtrl()

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
        #Helpbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))
        #HelpOverbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_info.png',    wx.BITMAP_TYPE_PNG))

        Playbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_start.png',    wx.BITMAP_TYPE_PNG))
        PlayOverbmp = wx.BitmapFromImage(wx.Image('Buttons/buttons_start.png',    wx.BITMAP_TYPE_PNG))

        bp = wx.Panel(p, -1, style=wx.NO_BORDER, size=(1920,1), pos=(0,1080-50))
        bp.SetBackgroundColour('#222222')

        CHbutton = gbuttons.GenBitmapButton(p, -1, CHbmp, style=wx.NO_BORDER,size=(120,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        #ENbutton   = wx.BitmapButton(p, -1, ENbmp,   style=wx.BU_EXACTFIT, size=(120,40))
        ENbutton = gbuttons.GenBitmapButton(p, -1, ENbmp, style=wx.NO_BORDER,size=(120,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        #Helpbutton = gbuttons.GenBitmapButton(p, -1, Helpbmp, style=wx.NO_BORDER,size=(40,40)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
        #Helpbutton = platebtn.PlateButton(p, -1, "", Helpbmp, style=platebtn.PB_STYLE_DEFAULT, size=(40,40))
        #Helpbutton = platebtn.PlateButton(p, -1, Helpbmp, style=wx.BU_EXACTFIT, size=(40,40))
        #Helpbutton.SetPressColor(wx.RED)
        #CHbutton.SetPressColor(wx.RED)
        CHbutton.SetBitmapSelected(CHOverbmp)
        ENbutton.SetBitmapSelected(ENOverbmp)
        #Helpbutton.SetBitmapSelected(HelpOverbmp)

        CHbutton.SetPosition((1920-50-130-130,1080-45))
        ENbutton.SetPosition((1920-50-130,1080-45))
        #Helpbutton.SetPosition((1920-50,1080-45))

        CHbutton.SetPosition((1920-130-130,1080-45))
        ENbutton.SetPosition((1920-130,1080-45))
        #Helpbutton.SetPosition((1920-50-500,1080-45))

        #CHbutton.SetPosition((1920-50-130-130,80-45))
        #ENbutton.SetPosition((1920-50-130,80-45))
        #Helpbutton.SetPosition((1920-50,80-45))

        self.SimIcon = None

        self.PlayBtn = gbuttons.GenBitmapButton(p, -1, Playbmp, style=wx.NO_BORDER,size=(40,40), pos=(1475,270))
        self.PlayBtn.Enable(False)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, self.PlayBtn)

        self.CHBtn = CHbutton
        self.ENBtn = ENbutton
        #self.HelpBtn = Helpbutton

        self.Bind(wx.EVT_BUTTON, self.OnButton)
        #self.Bind(wx.EVT_TOGGLEBUTTON, self.OnToggle)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, ENbutton)
        #self.Bind(wx.EVT_BUTTON, self.OnButton, Helpbutton)

        self.help_text = HelpText(p, INFOTEXT, (600,100), (1150,65))

        self.info_text = MovieInfoText(p, '', 20)
        self.info_text.SetBackgroundColour('BLACK')

        self.hilightbox = None
        self.sim_selected = None
        self.b_selected = None
        self.playing = None

        x,y = 100,20
        w,h = 125,125
        self.mbtns = []
        index = 0
        for name in ['EJS', 'CJS', 'EEJS', 'CJSECC']:
            for t in [1,2,3,5]:
                for p0 in [1,2]:
                    for m in [10,5]:
                        fname = 'thumbs2/cthumb_%s_t%ip%im%i.s.png' % (name,t,p0,m)
                        try:
                            bmp = wx.BitmapFromImage(wx.Image(fname, wx.BITMAP_TYPE_PNG))
                        except:
                            pass
                        j = index % 8;
                        i = index / 8;

                        b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_SQUARE, size=(110,110), pos=(x+w*j, y+h*i))
                        b.index = index
                        b.SetBackgroundColour(wx.BLACK)
                        b.SetPressColor(wx.RED)
                        b.SetLabelColor(wx.RED)
                        b.name = fname
                        b.mname = 'movies2/%s_t%ip%im%i.mpg' % (name,t,p0,m)
                        #self.Bind(wx.EVT_BUTTON, self.OnButton, b)
                        self.mbtns.append([b,bmp])
                        index += 1


                #b = gbuttons.GenBitmapButton(p, -1, bmp, style=wx.NO_BORDER,size=(110,110), pos=(x+w*j, y+h*i)) #, style=platebtn.PB_STYLE_DEFAULT, size=(120,40))
                #b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_DEFAULT, size=(110,110), pos=(x+w*j, y+h*i))
                #b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_SQUARE|platebtn.PB_STYLE_TOGGLE, size=(110,110), pos=(x+w*j, y+h*i))
                #b = platebtn.PlateButton(p, -1, "", bmp, style=platebtn.PB_STYLE_SQUARE, size=(110,110), pos=(x+w*j, y+h*i))
                #b.index = i * 8 + j
                #b.SetBackgroundColour(wx.BLACK)
                #b.SetPressColor(wx.RED)
                #b.SetLabelColor(wx.RED)
                #self.Bind(wx.EVT_BUTTON, self.OnButton, b)
                #self.mbtns.append([b,bmp])

    def OnButton(self, event):
        global lang
        obj = event.GetEventObject()
        if obj is self.CHBtn:
            print 'German!'
            if lang != 'deutsch': 
                lang = 'deutsch'
                self.redraw()

        elif obj is self.ENBtn:
            print 'English!'
            if lang != 'english': 
                lang = 'english'
                self.redraw()

        #elif obj is self.HelpBtn:
            #print 'Help!'

        elif obj is self.PlayBtn and self.sim_selected is not None:
            r,c = divmod(self.sim_selected, 8)
            print r,c
            self.mframe.Show()
            if self.playing is None or self.playing != self.b_selected.mname:
                self.mc.Load(self.b_selected.mname)
                print 'Would play', self.b_selected.mname
                #self.mc.Load('movie.mpg')
                self.playing = self.b_selected.mname
            else:
                self.mc.Stop()
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
                    self.b_selected = b
                else:
                    b.Enable(True)

    def OnQuit(self, event):
        print 'Quit from keyboard disabled.'

    def redraw(self):
        self.info_text.draw()
        self.help_text.draw()

class PCApp(wx.App):

    def OnInit(self):

        frame = PCFrame(None, -1, title=_('This should be hidden in Fullscreen'), size=(1920,1080), pos=(0,0), style=wx.FRAME_NO_TASKBAR)
        frame.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
        #frame.SetPosition((1920,0))
        frame.Show(True)
        self.frame = frame

        mframe = PCMovie(None, -1, title=_('This should be hidden in Fullscreen'), size=(1920,1080), pos=(0,-1080), style=wx.FRAME_NO_TASKBAR)
        #mframe = PCMovie(None, -1, title=_('This should be hidden in Fullscreen'), size=(1920,1080), pos=(1920,0), style=wx.FRAME_NO_TASKBAR)
        #frame.ShowFullScreen(True, style=wx.FULLSCREEN_ALL)
        #frame.SetPosition((2000,0))
        mframe.Show(True)

        frame.mframe = mframe
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
