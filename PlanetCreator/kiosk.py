# cut + paste comments
kUIModeNormal = 0
kUIModeContentSuppressed = 1
kUIModeContentHidden = 2
kUIModeAllSuppressed = 4
kUIModeAllHidden = 3
kUIOptionAutoShowMenuBar = 1 << 0
kUIOptionDisableAppleMenu = 1 << 2
kUIOptionDisableProcessSwitch = 1 << 3
kUIOptionDisableForceQuit = 1 << 4
kUIOptionDisableSessionTerminate = 1 << 5
kUIOptionDisableHide = 1 << 6
# wrapped function
import sys
sys.path.append('/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python')
sys.path.append('/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python/PyObjC')
import objc
from Foundation import NSBundle
bundle = NSBundle.bundleWithPath_('/System/Library/Frameworks/Carbon.framework')
objc.loadBundleFunctions(bundle, globals(), (
     ('SetSystemUIMode', 'III', " Sets the presentation mode for system-provided user interface elements."),
))

options = (  kUIOptionDisableAppleMenu 
           | kUIOptionDisableProcessSwitch 
           | kUIOptionDisableForceQuit 
           | kUIOptionDisableSessionTerminate 
           | kUIOptionDisableHide
          )
SetSystemUIMode(kUIModeAllHidden, options)

sys.path = sys.path[:-2]
