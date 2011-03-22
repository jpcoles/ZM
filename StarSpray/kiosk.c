#include <Carbon/Carbon.h>
#include "kiosk.h"

void kiosk()
{
    int options = kUIOptionDisableAppleMenu
                //| kUIOptionDisableProcessSwitch
                | kUIOptionDisableForceQuit
                | kUIOptionDisableSessionTerminate
                | kUIOptionDisableHide
    ;
    SetSystemUIMode(kUIModeAllHidden, options);
}

