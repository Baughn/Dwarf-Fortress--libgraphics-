#ifndef VIEWBASE_H
#define VIEWBASE_H

enum InterfaceBreakdownTypes
{
	INTERFACE_BREAKDOWN_NONE,
	INTERFACE_BREAKDOWN_QUIT,
	INTERFACE_BREAKDOWN_STOPSCREEN,
	INTERFACE_BREAKDOWN_TOFIRST,
	INTERFACE_BREAKDOWNNUM
};

class viewscreenst
{
	public:
		viewscreenst *child;
		viewscreenst *parent;
		char breakdownlevel;

		char option_key_pressed;
		virtual char is_legacy_screen(){return 1;}
		virtual void input(){}
		virtual void logic(){}
		virtual void view(){}
		virtual void render(){}

		virtual void help();
		virtual char movies_okay(){return 1;}
		virtual char is_option_screen(){return 0;}
		virtual char is_save_screen(){return 0;}
		viewscreenst()
			{
			child=0;
			parent=0;
			breakdownlevel=INTERFACE_BREAKDOWN_NONE;
			option_key_pressed=0;
			}
		virtual ~viewscreenst(){}
};

class ViewWidgets {
 protected:
 short x,x2,y,y2;
 public:
 char HasFocus;
 char DoClear;
 ViewWidgets(short X, short X2, short Y, short Y2) {x=X;y=Y;x2=X2;y2=Y2;HasFocus=DoClear=0;}
 virtual char input(void* param=0) {return 0;} //return value 1 on widget handled keypress, 0 otherwise
 virtual void logic(void* param=0) {}
 virtual void render(void* param=0) {}
 virtual void move(short X, short X2, short Y, short Y2) {x=X;y=Y;x2=X2;y2=Y2;}
 short clipx[2], clipy[2];
 void setclip();
 void releaseclip();
};

class ListItemCB {
 public:
 virtual const char* ListItem(int Index, void* Param, char* Store) {
  *Store=0;
  return Store;
  //Store does not necessarily have to be used in the code for derived
  //classes.  The return pointer must always be set to where the text is.
 }
};

class ListSelectorWidget : public ViewWidgets {
 protected:
 int count;
 int PageStart, PageEnd;
 public:
 char SelColorF, SelColorB, SelColorH; //unfocused selection colors
 char FSelColorF, FSelColorB, FSelColorH; //focused selection colors
 char ColorF, ColorB, ColorH; //other colors
 char wrap;  //list can wrap
 char ColorBlanks; //fill blank location with background color
 int selected; //treat as read only this will move into protected later
 ListItemCB* ListSrc;  //pointer to derived class that knows how to make the text items
 void SetListCount(int c) {
  //0 makes empty, negative numbers can be used to decrease list size
  if (c<0) {
   c=count+c;
   if (c<0) c=0;
  }
  int ps=PageSize();
  if (c) {
   if (selected==-1) selected=0;
   else if (selected>=c) selected=c-1;
   PageStart=(selected/ps)*ps;
   PageEnd=PageStart+ps;
  } else {
   selected=-1;
   PageStart=0;
   PageEnd=ps;
  }
  count=c;
 }
 virtual int GetCount() {return count;}
 virtual int PageSize() {return 1;}
 ListSelectorWidget(short X, short X2, short Y, short Y2) : ViewWidgets(X,X2,Y,Y2) {
  count=0;
  selected=-1;
  SelColorF=3;
  FSelColorF=3;
  ColorF=7;
  SelColorB=FSelColorB=ColorB=0;
  SelColorH=ColorH=0;
  FSelColorH=1;
  PageEnd=PageSize();
  PageStart=0;
  ColorBlanks=0;
  wrap=1;
 }
 virtual void Jump(int s) {
  if (s<0) return;
  if (s>=count) s=count-1;
  selected=s;
  if (s<0) return;
  int ps=PageSize();
  PageStart=(selected/ps)*ps;
  PageEnd=PageStart+ps;
 }
 virtual void move(short X, short X2, short Y, short Y2) {
  x=X;y=Y;x2=X2;y2=Y2;
  int ps=PageSize();
  PageStart=(selected/ps)*ps;
  PageEnd=PageStart+ps;
 }
};

class TabBarWidget : public ListSelectorWidget {
 short spacing;
 public:
 const int *ForwardKeyList, *ReverseKeyList;
 short FKeySize, RKeySize;
 TabBarWidget(short X, short X2, short Y, short Y2) : ListSelectorWidget(X,X2,Y,Y2) {
  ForwardKeyList=ReverseKeyList=0;
  FKeySize=RKeySize=0;
  spacing=1;
  PageEnd=PageSize();
 }
 virtual char input(void* param=0);
 virtual void render(void* param=0);
 virtual int PageSize() {
  if (spacing) return ((x2-x)/spacing)*(y2-y+1);
  else return 1;
 }
 void AdjustSpacing(short sp) {
  spacing=sp;
  int ps=PageSize();
  PageStart=(selected/ps)*ps;
  PageEnd=PageStart+ps;
 }
};

class ScrollListWidget : public ListSelectorWidget {
 public:
 const int *DownKeyList, *UpKeyList, *PgDnKeyList, *PgUpKeyList;
 short DKeySize, UKeySize, PDKeySize, PUKeySize;
 ScrollListWidget(short X, short X2, short Y, short Y2) : ListSelectorWidget(X,X2,Y,Y2) {
  DownKeyList=UpKeyList=PgDnKeyList=PgUpKeyList=0;
  DKeySize=UKeySize=PDKeySize=PUKeySize=0;
 }
 virtual char input(void* param=0);
 virtual void render(void* param=0);
 virtual int PageSize() {return y2-y-1;}
};
#endif
