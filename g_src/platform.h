#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef WIN32
# include <windows.h>
enum {
	VK_WHEELBASE = 0xb0,
	VK_BUTTON3 = 0xb3,
	VK_WHEELUP = 0xb4,
	VK_WHEELDOWN,
	VK_WHEELLEFT,
	VK_WHEELRIGHT,
};
#else

enum {
	VK_CANCEL     = 0x03,
	VK_BACK       = 0x08,
	VK_TAB        = 0x09,
	VK_CLEAR      = 0x0c,
	VK_RETURN     = 0x0d,

	VK_SHIFT      = 0x10,
	VK_CONTROL,
	VK_MENU,
	VK_PAUSE,
	VK_CAPITAL,
	VK_ESCAPE     = 0x1b,
	
	VK_SPACE      = 0x20,
	VK_PRIOR,
	VK_NEXT,
	VK_END,
	VK_HOME,
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN,
	VK_SELECT,
	VK_PRINT,
	VK_EXECUTE,
	VK_SNAPSHOT,
	VK_INSERT,
	VK_DELETE,
	VK_HELP,

	VK_NUMPAD0    = 0x60,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	VK_MULTIPLY,
	VK_ADD,
	VK_SEPARATOR,
	VK_SUBTRACT,
	VK_DECIMAL,
	VK_DIVIDE,

	VK_F1         = 0x70,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_F13,
	VK_F14,
	VK_F15,
	VK_F16,
	VK_F17,
	VK_F18,
	VK_F19,
	VK_F20,
	VK_F21,
	VK_F22,
	VK_F23,
	VK_F24,

	VK_NUMLOCK    = 0x90,
	VK_SCROLL,

	VK_LSHIFT     = 0xa0,
	VK_RSHIFT,
	VK_LCONTROL,
	VK_RCONTROL,
	VK_LMENU,
	VK_RMENU,

	VK_WHEELBASE = 0xb0,
	VK_BUTTON3 = 0xb3,
	VK_WHEELUP = 0xb4,
	VK_WHEELDOWN,
	VK_WHEELLEFT,
	VK_WHEELRIGHT,
};

enum {
	// NOTE: These probably don't match Windows values.
	MB_OK    = 0x01,
	MB_YESNO = 0x02,
	MB_ICONQUESTION    = 0x10,
	MB_ICONEXCLAMATION = 0x20,

	IDOK = 1,
	IDNO,
	IDYES,
};


typedef int HANDLE;
typedef HANDLE HINSTANCE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HANDLE HGLRC;

#ifndef HWND_DESKTOP
#define HWND_DESKTOP ((HWND)-1)
#endif


typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef unsigned int UINT;
typedef short SHORT;
typedef long LONG;
typedef long long LONGLONG;

typedef WORD WPARAM;
typedef DWORD LPARAM;


typedef struct {
	LONG x;
	LONG y;
} POINT;

typedef union {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef struct {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG;


DWORD GetTickCount();	// returns ms since system startup
BOOL CreateDirectory(const char* pathname, void*);
BOOL DeleteFile(const char* filename);
void ZeroMemory(void* dest, int len);
BOOL QueryPerformanceCounter(LARGE_INTEGER* performanceCount);
BOOL QueryPerformanceFrequency(LARGE_INTEGER* performanceCount);
int MessageBox(HWND *dummy, const char* text, const char* caption, UINT type);
char* itoa(int value, char* result, int base);

#endif // WIN32

SHORT Enabler_GetKeyState(int virtKey);
int Enabler_ShowCursor(BOOL show);

#endif // _PLATFORM_H_

