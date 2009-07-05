#include "platform.h"
#include <string.h>

#include "svector.h"
#include <string>
using std::string;

#include "random.h"


#include "g_basics.h"
#include "graphics.h"
extern graphicst gps;

#include "enabler.h"

#include "music_and_sound_g.h"

#include "interface.h"
extern interfacest gview;

void ViewWidgets::setclip() {
 clipx[0]=gps.clipx[0];
 clipx[1]=gps.clipx[1];
 clipy[0]=gps.clipy[0];
 clipy[1]=gps.clipy[1];
 gps.setclipping(x,x2,y,y2);
}

void ViewWidgets::releaseclip(){
 gps.setclipping(clipx[0],clipx[1],clipy[0],clipy[1]);
}

char TabBarWidget::input(void* param) {
 if (!count) return 0;
 if (!HasFocus) return 0;
 int ps=PageSize();
 if (gview.pressedList(ForwardKeyList, FKeySize)) {
  selected++;
  if (selected==count) {
   if (wrap) {
    selected=0;
    PageStart=0;
    PageEnd=ps;
   } else selected--;
  } else if (selected==PageEnd) {
   PageStart+=ps;
   PageEnd+=ps;
  }
  return 1;
 }
 if (gview.pressedList(ReverseKeyList, RKeySize)) {
  selected--;
  if (selected==-1) {
   if (wrap) {
    selected=count-1;
    PageStart=(selected/ps)*ps;
    PageEnd=PageStart+ps;
   } else selected=0;
  }
  else if (selected<PageStart) {
   PageStart-=ps;
   PageEnd-=ps;
  }
  return 1;
 }
 return 0;
}

void TabBarWidget::render(void* param) {
 setclip();
 if (DoClear) gps.erasescreen_clip();
 int X=x;
 int Y=y;
 int i=PageStart;
 gps.locate(Y,X);
 char store[256];
 while (i<PageEnd) {
  if (i==count) break;
  const char* str=ListSrc->ListItem(i,param,store);
  int len=spacing;
  if (X+len>=x2) {
   X=x;
   ++Y;
   if (Y>y2) break;
   gps.locate(Y,X);
  }
  if (i==selected) {
   if (HasFocus) gps.changecolor(FSelColorF, FSelColorB, FSelColorH);
   else gps.changecolor(SelColorF, SelColorB, SelColorH);
  } else gps.changecolor(ColorF, ColorB, ColorH);
  while (len) {
   gps.addchar(*str,1);
   if (*str) {
    str++;
    if (!*str) if (!ColorBlanks) {
     X=gps.screenx;
     gps.locate(Y,X+len);
     break;
    }
   }
   --len;
  }
  ++i;
  X=gps.screenx;
 }
 releaseclip();
}

char ScrollListWidget::input(void* param) {
 if (!count) return 0;
 if (!HasFocus) return 0;
 int ps=PageSize();
 if (gview.pressedList(DownKeyList, DKeySize)) {
  selected++;
  if (selected==count) {
   if (!wrap) selected--;
   else {
    PageStart=selected=0;
    PageEnd=ps;
   }
  }
  else if (selected==PageEnd) {
   PageStart+=ps;
   PageEnd+=ps;
  }
  return 1;
 }
 if (gview.pressedList(UpKeyList, UKeySize)) {
  selected--;
  if (selected<0) {
   if (!wrap) selected=0;
   else {
    selected+=count;
    PageStart=(selected/ps)*ps;
    PageEnd=PageStart+ps;
   }
  }
  else if (selected<PageStart) {
   PageStart-=ps;
   PageEnd-=ps;
  }
  return 1;
 }
 if (gview.pressedList(PgDnKeyList, PDKeySize)) {
  if (wrap) {
   if (selected==count-1) {
    PageStart=selected=0;
    PageEnd=ps;
   } else selected+=ps;
  }
  else selected+=ps;
  if (selected>=count) selected=count-1;
  if (selected>=PageEnd) {
   PageStart+=ps;
   PageEnd+=ps;
  }
  return 1;
 }
 if (gview.pressedList(PgUpKeyList, PUKeySize)) {
  if (wrap) {
   if (!selected) {
    selected=count-1;
    PageStart=(selected/ps)*ps;
    PageEnd=PageStart+ps;
   } else selected-=ps;
  } else selected-=ps;
  if (selected<0) selected=0;
  if (selected<PageStart) {
   PageStart-=ps;
   PageEnd-=ps;
  }
  return 1;
 }
 return 0;
}

void ScrollListWidget::render(void* param) {
 setclip();
 if (DoClear) gps.erasescreen_clip();
 int X=x;
 int Y=y;
 int i=PageStart;
 gps.locate(Y,X);
 char store[256];
 while (i<PageEnd) {
  if (i==count) break;
  const char* str=ListSrc->ListItem(i,param,store);
  if (i==selected) {
   if (HasFocus) gps.changecolor(FSelColorF, FSelColorB, FSelColorH);
   else gps.changecolor(SelColorF, SelColorB, SelColorH);
  } else gps.changecolor(ColorF, ColorB, ColorH);
  while (1) {
   gps.addchar(*str,1);
   if (*str) {
    str++;
    if (!(*str)) if (!ColorBlanks) break;
   }
   if (gps.screenx==x2) break;
  }
  ++i;
  gps.locate(++Y,X);
 }
 releaseclip();
}
//To do
//add scroll indicators, vertical bar, arrows, etc
//make it check mouse position for in scroll region for wheel
//left click for select, needs seperate binding or something
//border option for regions
//string entry widget
