                        /*** /

This file is part of Golly, a Game of Life Simulator.
Copyright (C) 2010 Andrew Trevorrow and Tomas Rokicki.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 Web site:  http://sourceforge.net/projects/golly
 Authors:   rokicki@gmail.com  andrew@trevorrow.com

                        / ***/
#ifndef _WXSTATUS_H_
#define _WXSTATUS_H_

#include "bigint.h"     // for bigint

// Define a child window for status bar at top of main frame:

class StatusBar : public wxWindow
{
public:
    StatusBar(wxWindow* parent, wxCoord xorg, wxCoord yorg, int wd, int ht);
    ~StatusBar();

   void ClearMessage();
   // erase bottom line of status bar

   void DisplayMessage(const wxString& s);
   // display message on bottom line of status bar

   void ErrorMessage(const wxString& s);
   // beep and display message on bottom line of status bar

   void SetMessage(const wxString& s);
   // set message string without displaying it (until next update)

   void UpdateXYLocation();
   // XY location needs to be updated

   void CheckMouseLocation(bool active);
   // check location of mouse and update XY location if necessary

   wxString Stringify(const bigint& b);
   // convert given number to string suitable for display
   
   int GetCurrentDelay();
   // return current delay (in millisecs)

   wxFont* GetStatusFont() { return statusfont; }
   int GetTextAscent() { return textascent; }

   int statusht;
   // status bar height (0 if not visible, else STATUS_HT or STATUS_EXHT)
   
private:
   // any class wishing to process wxWidgets events must use this macro
   DECLARE_EVENT_TABLE()

   // event handlers
   void OnPaint(wxPaintEvent& event);
   void OnMouseDown(wxMouseEvent& event);
   void OnEraseBackground(wxEraseEvent& event);

   bool ClickInGenBox(int x, int y);
   bool ClickInScaleBox(int x, int y);
   bool ClickInStepBox(int x, int y);
   void SetStatusFont(wxDC& dc);
   void DisplayText(wxDC& dc, const wxString& s, wxCoord x, wxCoord y);
   void DrawStatusBar(wxDC& dc, wxRect& updaterect);

   wxBitmap* statbitmap;         // status bar bitmap
   int statbitmapwd;             // width of status bar bitmap
   int statbitmapht;             // height of status bar bitmap
   
   int h_gen;                    // horizontal position of "Generation"
   int h_pop;                    // horizontal position of "Population"
   int h_scale;                  // horizontal position of "Scale"
   int h_step;                   // horizontal position of "Step"
   int h_xy;                     // horizontal position of "XY"
   int textascent;               // vertical adjustment used in DrawText calls
   wxString statusmsg;           // for messages on bottom line
   bigint currx, curry;          // cursor location in cell coords
   bool showxy;                  // show cursor's XY location?
   wxFont* statusfont;           // status bar font
};

extern const int STATUS_HT;      // normal status bar height
extern const int STATUS_EXHT;    // height when showing exact numbers

#endif
