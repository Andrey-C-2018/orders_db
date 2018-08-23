#pragma once
#include <windows.h>
#include <memory>
#include <basic/tstring.h>

enum {
	RESULT_SUCCESS = S_OK, \
	RESULT_FAIL = E_FAIL
};

#ifndef GET_KEYSTATE_WPARAM
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#endif

typedef HWND _plHWND;
inline void _plUnsetHWND(_plHWND &hwnd);
inline bool _plIsHWNDSet(_plHWND hwnd);

typedef WNDPROC _plWNDPROC;

typedef HDC _plHDC;
typedef PAINTSTRUCT _plContextParams;
inline void _plUnsetHDC(_plHDC &dc);
inline bool _plIsHDCSet(_plHDC &dc);

typedef UINT _plEventId;
typedef UINT _plNotificationCode;

class XWindow;

enum Events{
	EVT_CREATE = WM_CREATE,\
	EVT_PAINT = WM_PAINT,\
	EVT_DESTROY = WM_DESTROY,\
	EVT_COMMAND = WM_COMMAND,\
	EVT_SIZE = WM_SIZE,\
	EVT_MOVE = WM_MOVE,\
	EVT_ERASEBKGND = WM_ERASEBKGND,\
	EVT_CHAR = WM_CHAR,\
	EVT_KEYDOWN = WM_KEYDOWN,\
	EVT_LBUTTONDOWN = WM_LBUTTONDOWN,\
	EVT_MOUSEWHEEL = WM_MOUSEWHEEL,\
	EVT_HSCROLL = WM_HSCROLL,\
	EVT_VSCROLL = WM_VSCROLL
};

enum NotificationCodes{
	NCODE_BUTTON_CLICKED = BN_CLICKED,\
	NCODE_EDIT_CHANGED = EN_CHANGE, \
	NCODE_EDIT_SET_FOCUS = EN_SETFOCUS, \
	NCODE_EDIT_LOOSE_FOCUS = EN_KILLFOCUS, \
	NCODE_COMBOBOX_SELCHANGED = CBN_SELCHANGE, \
	NCODE_COMBOBOX_LOOSE_FOCUS = CBN_KILLFOCUS
};

const unsigned EVT_DONT_PROCESS = 1;

#define _plCallbackFnParams HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
#define _plCallbackFnParamsList hWnd, message, wParam, lParam
#define _plCallbackFnRetValue LRESULT
#define _plCallbackFnModifier CALLBACK

#define _plEventParams WPARAM wParam, LPARAM lParam
#define _plEventParamsList WPARAM wParam, LPARAM lParam
struct _plEventInfo{
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	_plEventInfo() noexcept : message(0), wParam(0), lParam(0){ }
	inline _plEventInfo(UINT msg, WPARAM wparam, LPARAM lparam) noexcept: \
						message(msg), wParam(wparam), lParam(lparam){ }
};

#define _plMAIN_FN_MODIFIERS WINAPI
#define _plMainFnParams HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd
#define _plMainFnParamsList hInstance, hPrevInstance, lpCmdLine, nShowCmd
#define _plMainEntryPoint(_plMainFnParams) WinMain(_plMainFnParams)

#define _plDEFAULT_COORD CW_USEDEFAULT

class CGlobalParams{
	static std::shared_ptr<CGlobalParams> instance;
	HINSTANCE hInst;
	CGlobalParams() noexcept { }
public:
	inline static std::shared_ptr<CGlobalParams> GetInstance(){

		if(!instance) instance.reset(new CGlobalParams());
		return instance;
	}
	inline void SetHInstance(HINSTANCE hInst){ this->hInst = hInst; }
	inline HINSTANCE GetHInstance() const{ return this->hInst; }

	inline static void Destroy() {

		if (instance && instance.unique()) instance.reset();
	}
};

inline void _plInit(_plMainFnParams);
inline void _plPlatformSpecificCleanup();
inline int _plLaunchMessageLoop();
inline _plCallbackFnRetValue _plCallbackFnModifier _plDefaultEventAction(_plCallbackFnParams);
inline _plCallbackFnRetValue _plCallbackFnModifier _plDefaultEventAction(_plWNDPROC wnd_proc, _plCallbackFnParams);
inline _plWNDPROC _plSetWindowProc(_plHWND hwnd, _plWNDPROC new_proc);

inline int _plGetWindowId(_plCallbackFnParams);
inline _plEventId _plGetEventId(_plCallbackFnParams);
inline _plNotificationCode _plGetNotificationCode(_plCallbackFnParams);
inline _plHWND _plGetWindowHandle(_plCallbackFnParams);
inline XWindow *_plCreateEventExtractWindowObject(_plCallbackFnParams);
inline _plEventInfo _plInitEventInfo(_plCallbackFnParams);
inline void _plSendMessage(_plHWND hwnd, const _plEventInfo &event_info);

enum SizeParams {
	FL_MAXIMIZED = SIZE_MAXIMIZED, \
	FL_MINIMIZED = SIZE_MINIMIZED, \
	FL_RESTORED = SIZE_RESTORED
};

enum WindowFlags{
	FL_WINDOW_VISIBLE = WS_VISIBLE,\
	FL_WINDOW_BORDERED = WS_BORDER,\
	FL_WINDOW_CLIPCHILDREN = WS_CLIPCHILDREN,\
	FL_WINDOW_CLIPSIBLINGS = WS_CLIPSIBLINGS
};

enum EditFlags{
	FL_EDIT_AUTOHSCROLL = ES_AUTOHSCROLL,\
	FL_EDIT_LEFT = ES_LEFT,\
	FL_EDIT_CENTER = ES_CENTER,\
	FL_EDIT_RIGHT = ES_RIGHT,\
};

enum ComboboxFlags{
	FL_COMBOBOX_DROPDOWN = CBS_DROPDOWN | WS_VSCROLL,\
	FL_COMBOBOX_SIMPLE = CBS_SIMPLE
};

struct XPoint : public POINT {

	XPoint() noexcept { x = 0; y = 0; }
	XPoint(const int x, const int y) noexcept {

		this->x = x;
		this->y = y;
	}
	XPoint(const XPoint &obj) noexcept { 
		
		this->x = obj.x;
		this->y = obj.y;
	}
	XPoint(XPoint &&obj) noexcept {

		this->x = obj.x;
		this->y = obj.y;
		obj.x = obj.y = 0;
	}
	XPoint &operator=(const XPoint &obj) noexcept {

		this->x = obj.x;
		this->y = obj.y;
		return *this;
	}
	XPoint &operator=(XPoint &&obj) noexcept {

		this->x = obj.x;
		this->y = obj.y;
		obj.x = obj.y = 0;
		return *this;
	}
	inline bool operator==(const XPoint &obj) const {

		return x == obj.x && y == obj.y;
	}
	inline bool operator!=(const XPoint &obj) const {

		return !operator==(obj);
	}
};

struct XRect : public RECT {

	XRect() noexcept { left = right = top = bottom = 0; }
	XRect(const int left, const int top, \
		const int right, const int bottom) noexcept {

		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	XRect(const XRect &obj) noexcept {

		this->left = obj.left;
		this->top = obj.top;
		this->right = obj.right;
		this->bottom = obj.bottom;
	}
	XRect(XRect &&obj) noexcept {

		this->left = obj.left;
		this->top = obj.top;
		this->right = obj.right;
		this->bottom = obj.bottom;
		obj.left = obj.right = obj.top = obj.bottom = 0;
	}
	XRect &operator=(const XRect &obj) noexcept {

		this->left = obj.left;
		this->top = obj.top;
		this->right = obj.right;
		this->bottom = obj.bottom;
		return *this;
	}
	XRect &operator=(XRect &&obj) noexcept {

		this->left = obj.left;
		this->top = obj.top;
		this->right = obj.right;
		this->bottom = obj.bottom;
		obj.left = obj.right = obj.top = obj.bottom = 0;
		return *this;
	}
	inline bool operator==(const XRect &obj) const {

		return left == obj.left && top == obj.top && \
			right == obj.right && bottom == obj.bottom;
	}
	inline bool operator!=(const XRect &obj) const {

		return !operator==(obj);
	}
	inline bool is_null() const {

		return left == 0 && right == 0 && \
				top == 0 && bottom == 0;
	}
};

struct XSize {
	int width;
	int height;

	XSize() noexcept : width(0), height(0){ }

	XSize(const int width_, const int height_) noexcept : \
			width(width_), height(height_){	}

	XSize(const XSize &obj) noexcept : \
			width(obj.width), height(obj.height) { }

	XSize(XSize &&obj) noexcept : \
		width(obj.width), height(obj.height) {
		
		obj.width = obj.height = 0;
	}
	XSize &operator=(const XSize &obj) noexcept {

		width = obj.width;
		height = obj.height;
		return *this;
	}
	XSize &operator=(XSize &&obj) noexcept {

		width = obj.width;
		height = obj.height;
		obj.width = obj.height = 0;
		return *this;
	}
	inline bool operator==(const XSize &obj) const {

		return width == obj.width && obj.height == 0;
	}
	inline bool operator!=(const XSize &obj) const {

		return !operator==(obj);
	}
	inline bool is_null() const {

		return width == 0 && height == 0;
	}
};

inline bool _plRegisterWidget(const Tchar *class_name, WNDPROC WndProc);
inline bool _plRegisterWindow(const Tchar *class_name, WNDPROC WndProc);
inline _plHWND _plCreateWidget(_plHWND hwnd_parent, const Tchar *class_name, \
							const int flags, const int X, const int Y,\
							const int width, const int height,\
							const Tchar *label, const int id, \
							XWindow *this_window);
inline _plHWND _plCreateWindow(const Tchar *class_name, \
							   const int flags, const int X, const int Y,\
								const int width, const int height,\
								const Tchar *label,\
								XWindow *this_window);
inline void _plWindowPostCreate(_plHWND hwnd);
inline void _plSetFocus(_plHWND hwnd);
inline size_t _plGetLabelSize(_plHWND hwnd);
inline void _plGetLabel(_plHWND hwnd, Tchar *label, const size_t max_size);
inline bool _plSetLabel(_plHWND hwnd, const Tchar *label);
inline void _plInvalidate(_plHWND hwnd, const XRect *rect, const bool redraw);
inline void _plEnableWindow(_plHWND hwnd, const bool enable);
inline bool _plDestroyWindow(_plHWND hwnd);
inline int _plGetWidth(_plCallbackFnParams);
inline int _plGetHeight(_plCallbackFnParams);
inline int _plGetSizeParam(_plCallbackFnParams);
inline void _plGetClientRect(_plHWND hwnd, XRect &rc);
inline void _plMoveWindow(_plHWND hwnd, int x, int y, int width, int height);
inline void _plShowWindow(_plHWND hwnd, bool show);

enum XVirtualKeys{
	X_VKEY_CTRL = MK_CONTROL,\
	X_VKEY_SHIFT = MK_SHIFT,\
	X_VKEY_ESCAPE = VK_ESCAPE,\
	X_VKEY_ENTER = VK_RETURN,\
	X_VKEY_LEFT = VK_LEFT,\
	X_VKEY_RIGHT = VK_RIGHT,\
	X_VKEY_UP = VK_UP,\
	X_VKEY_DOWN = VK_DOWN,\
	X_VKEY_PGUP = 33,\
	X_VKEY_PGDOWN = 34,\
	X_VKEY_HOME = VK_HOME,\
	X_VKEY_END = VK_END,\
	X_VKEY_LSHIFT = VK_LSHIFT,\
	X_VKEY_RSHIFT = VK_RSHIFT,\
	X_VKEY_LCTRL = VK_LCONTROL,\
	X_VKEY_RCTRL = VK_RCONTROL,\
	X_VKEY_LMENU = VK_LMENU,\
	X_VKEY_RMENU = VK_RMENU
};

enum XVirtualKeyState{
	X_VKEY_PUSHED = 0x8000,\
	X_VKEY_TOGGLED = 0x80
};

enum XMouseButtons{
	X_MOUSE_LBUTTON = MK_LBUTTON,\
	X_MOUSE_MBUTTON = MK_MBUTTON,\
	X_MOUSE_RBUTTON = MK_RBUTTON
};

inline Tchar _plGetChar(_plCallbackFnParams);
inline int _plGetKey(_plCallbackFnParams);
inline int _plGetVKeyState(int virtual_key);

inline int _plGetX(_plCallbackFnParams);
inline int _plGetY(_plCallbackFnParams);
inline int _plGetMouseVKey(_plCallbackFnParams);
inline int _plGetMouseWheelDelta(_plCallbackFnParams);

typedef HGDIOBJ _plGCObject;
typedef HPEN _plHPEN;
typedef HBRUSH _plHBRUSH;
typedef COLORREF _plCOLOR;
typedef HFONT _plHFONT;

enum XPenStyles{
	XPEN_SOLID = PS_SOLID,\
	XPEN_DASH = PS_DASH,\
	XPEN_NULL = PS_NULL
};

enum XSpecialPens{
	XSPECIAL_PEN_NULL = NULL_PEN,\
	XSPECIAL_PEN_WHITE = WHITE_PEN,\
	XSPECIAL_PEN_BLACK = BLACK_PEN
};

enum XSpecialBrushes{
	XSPECIAL_BRUSH_NULL = NULL_BRUSH,\
	XSPECIAL_BRUSH_WHITE = WHITE_BRUSH,\
	XSPECIAL_BRUSH_BLACK = BLACK_BRUSH
};

enum XBackgroundFillModes{
	X_BK_TRANSPARENT = TRANSPARENT,\
	X_BK_OPAQUE = OPAQUE
};

enum TextOutModes{
	XTEXTOUT_CLIPPED = ETO_CLIPPED,\
	XTEXTOUT_OPAQUE = ETO_OPAQUE
};

class _plXTextMetric{
	TEXTMETRIC tm;
public:
	inline _plXTextMetric(_plHDC dc){ GetTextMetrics(dc, &tm); }
	inline int GetMaxCharWidth() const{ return tm.tmMaxCharWidth; }
	inline int GetAverageCharWidth() const{ return tm.tmAveCharWidth; }
	inline int GetCharHeight() const{ return tm.tmHeight; }
};

typedef DWORD _plFONT_CHARSET;

enum FontProperties{
	X_ITALIC = 1,\
	X_UNDERLINE = 2,\
	X_STRIKEOUT = 4,\
	X_BOLD = 8
};

inline _plHDC _plGetHDC(_plHWND hwnd, _plContextParams *params);
inline void _plReleaseHDC(_plHWND hwnd, _plContextParams *params);
inline _plHDC _plGetStandAloneHDC(_plHWND hwnd);
inline void _plReleaseStandAloneHDC(_plHWND hwnd, _plHDC dc);

inline _plCOLOR _plGetColorRGB(unsigned char red, unsigned char green, unsigned char blue);
inline void _plTextOut(_plHDC dc, int x, int y, const Tchar *text, const size_t len);
inline void _plTextOut(_plHDC dc, int x, int y, int options, XRect &rc, const Tchar *text, const size_t len);
inline void _plRectangle(_plHDC dc, int left, int top, int right, int bottom);
inline void _plPolygon(_plHDC dc, const XPoint *points, const size_t count);
inline _plHPEN _plCreatePen(int style, int width, unsigned char red, unsigned char green, unsigned char blue);
inline _plHPEN _plCreateSpecialPen(const int pen_id);
inline _plHBRUSH _plCreateBrush(unsigned char red, unsigned char green, unsigned char blue);
inline _plHBRUSH _plCreateSpecialBrush(const int brush_id);
inline void _plReleaseObject(_plGCObject obj);
inline _plGCObject _plSelectObject(_plHDC dc, _plGCObject obj);
inline int _plSetBackgroundFillMode(_plHDC dc, int mode);
inline _plCOLOR _plSetBackgroundColor(_plHDC dc, _plCOLOR color);
inline _plHFONT _plCreateFont(int size, int rotation_angle, int symbols_angle, \
						int properties, _plFONT_CHARSET charset, const Tchar *lpszFace);

enum ScrollOrientation{
	X_SCROLL_HORZ = SB_HORZ,\
	X_SCROLL_VERT = SB_VERT,\
};

enum ScrollTypes{
	X_SCROLL_THUMBTRACK = SB_THUMBTRACK,\
	X_SCROLL_LINEUP = SB_LINEUP,\
	X_SCROLL_LINEDOWN = SB_LINEDOWN,\
	X_SCROLL_PAGEUP = SB_PAGEUP,\
	X_SCROLL_PAGEDOWN = SB_PAGEDOWN, \
	X_SCROLL_END = SB_ENDSCROLL
};

typedef SCROLLINFO _plSCROLLINFO;
struct XScrollParams {
	int max;
	int page;
	int pos;
};

inline int _plGetScrollThumbPos(_plCallbackFnParams);
inline int _plGetScrollThumbPosLarge(_plCallbackFnParams);
inline int _plGetScrollType(_plCallbackFnParams);
inline void _plSetScrollInfo(_plHWND hwnd, const int scroll_orient, const int max,\
						   const int page, const int pos);
inline void _plSetScrollInfo(_plHWND hwnd, const int scroll_orient, \
							const XScrollParams &params);
inline int _plGetScrollPos(_plHWND hwnd, const int scroll_orient);

inline void _plComboBoxAddItem(_plHWND hwnd, const Tchar* item);
inline void _plComboBoxDelItem(_plHWND hwnd, const size_t index);
inline size_t _plComboBoxGetCurrSel(_plHWND hwnd);
inline void _plComboBoxSetSel(_plHWND hwnd, const size_t index);
inline size_t _plComboBoxGetItemTextLen(_plHWND hwnd, const size_t index);
inline void _plComboBoxGetItemText(_plHWND hwnd, const size_t index, Tchar *text);
inline void _plComboBoxReset(_plHWND hwnd);

inline void _plMessageBox(const Tchar *message);
inline void _plMessageBoxANSI(const char *message);
inline void ErrorBox(const Tchar *message);
inline void ErrorBoxANSI(const char *message);

inline const Tchar *GetExecutableFullPath(Tchar *buffer);

//****************************************************************************

void _plUnsetHWND(_plHWND &hwnd){

hwnd = 0;
}

bool _plIsHWNDSet(_plHWND hwnd){

return (hwnd != 0);
}

void _plUnsetHDC(_plHDC &dc) {

dc = 0;
}

bool _plIsHDCSet(_plHDC &dc) {

return (dc != 0);
}

void _plInit(_plMainFnParams){

CGlobalParams::GetInstance()->SetHInstance(hInstance);
}

void _plPlatformSpecificCleanup(){ }

int _plLaunchMessageLoop(){
MSG msg;

while(GetMessage(&msg, 0, 0, 0)){
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

return (int)msg.wParam;
}

_plCallbackFnRetValue _plCallbackFnModifier _plDefaultEventAction(_plCallbackFnParams){

return DefWindowProc(_plCallbackFnParamsList);
}

_plCallbackFnRetValue _plCallbackFnModifier _plDefaultEventAction(_plWNDPROC wnd_proc, _plCallbackFnParams){

return CallWindowProc(wnd_proc, _plCallbackFnParamsList);
}

_plWNDPROC _plSetWindowProc(_plHWND hwnd, _plWNDPROC new_proc){

return (_plWNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)new_proc);
}

int _plGetWindowId(_plCallbackFnParams){

return ((int)(message == WM_COMMAND) * LOWORD(wParam));
}

_plEventId _plGetEventId(_plCallbackFnParams){

return message;
}

_plNotificationCode _plGetNotificationCode(_plCallbackFnParams){

return ((int)(message == WM_COMMAND) * HIWORD(wParam));
}

_plHWND _plGetWindowHandle(_plCallbackFnParams){

return hWnd;
}

XWindow *_plCreateEventExtractWindowObject(_plCallbackFnParams){
CREATESTRUCT *cr;

cr = (CREATESTRUCT *)lParam;
return (XWindow *)cr->lpCreateParams;
}

_plEventInfo _plInitEventInfo(_plCallbackFnParams){

return _plEventInfo(message, wParam, lParam);
}

void _plSendMessage(_plHWND hwnd, const _plEventInfo &event_info){

SendMessage(hwnd, event_info.message, event_info.wParam, event_info.lParam);
}

bool _plRegisterWidget(const Tchar *class_name,\
					   WNDPROC WndProc){
WNDCLASS w;
auto GlobalParams = CGlobalParams::GetInstance();

ZeroMemory(&w, sizeof(WNDCLASS));
w.hInstance = GlobalParams->GetHInstance();
w.style = CS_VREDRAW | CS_HREDRAW;
w.lpfnWndProc = WndProc;
w.lpszClassName = class_name;
w.lpszMenuName = 0;
w.hIcon = LoadIcon(0, IDI_APPLICATION);
w.hCursor = LoadCursor(0, IDC_ARROW);
w.hbrBackground = (HBRUSH)COLOR_BTNSHADOW;
w.cbClsExtra = 0;
w.cbWndExtra = 0;

return (RegisterClass(&w) != 0);
}

bool _plRegisterWindow(const Tchar *class_name,\
					   WNDPROC WndProc){
WNDCLASSEX w;
auto GlobalParams = CGlobalParams::GetInstance();

ZeroMemory(&w, sizeof(WNDCLASSEX));
w.cbSize = sizeof(WNDCLASSEX);
w.hInstance = GlobalParams->GetHInstance();
w.style = CS_OWNDC | CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
w.lpfnWndProc = WndProc;
w.lpszClassName = class_name;
w.lpszMenuName = 0;
w.hIcon = LoadIcon(0, IDI_APPLICATION);
w.hIconSm = LoadIcon(0, IDI_APPLICATION);
w.hCursor = LoadCursor(0, IDC_ARROW);
w.hbrBackground = (HBRUSH)COLOR_BTNSHADOW;
w.cbClsExtra = 0;
w.cbWndExtra = 0;

return (RegisterClassEx(&w) != 0);
}

_plHWND _plCreateWidget(_plHWND hwnd_parent, const Tchar *class_name, \
							const int flags, const int X, const int Y,\
							const int width, const int height,\
							const Tchar *label, const int id, \
							XWindow *this_window){
auto GlobalParams = CGlobalParams::GetInstance();

#pragma warning(push)
#pragma warning(disable: 4312)

return CreateWindow(class_name, label, WS_CHILD | flags,\
					 X, Y, width, height, hwnd_parent, (HMENU) id,\
					 GlobalParams->GetHInstance(), (LPVOID)this_window);
#pragma warning(pop)
}

_plHWND _plCreateWindow(const Tchar *class_name, \
							   const int flags, const int X, const int Y,\
								const int width, const int height,\
								const Tchar *label, \
								XWindow *this_window){
auto GlobalParams = CGlobalParams::GetInstance();

return CreateWindowEx(0, class_name, label, WS_OVERLAPPEDWINDOW | flags,
					X, Y, width, height, 0, 0,\
					GlobalParams->GetHInstance(), (LPVOID)this_window);
}

void _plWindowPostCreate(_plHWND hwnd){

ShowWindow(hwnd, SW_SHOWDEFAULT);
UpdateWindow(hwnd);
}

void _plSetFocus(_plHWND hwnd){

SetFocus(hwnd);
}

size_t _plGetLabelSize(_plHWND hwnd){

return GetWindowTextLength(hwnd);
}

void _plGetLabel(_plHWND hwnd, Tchar *label, const size_t max_size){

GetWindowText(hwnd, label, (int)max_size);
}

bool _plSetLabel(_plHWND hwnd, const Tchar *label){

return (SetWindowText(hwnd, label) != 0);
}

void _plInvalidate(_plHWND hwnd, const XRect *rect, const bool redraw){

InvalidateRect(hwnd, rect, redraw);
}

void _plEnableWindow(_plHWND hwnd, const bool enable) {

EnableWindow(hwnd, enable);
}

bool _plDestroyWindow(_plHWND hwnd){

return (DestroyWindow(hwnd) != 0);
}

int _plGetWidth(_plCallbackFnParams){

return LOWORD(lParam);
}

int _plGetHeight(_plCallbackFnParams){

return HIWORD(lParam);
}

int _plGetSizeParam(_plCallbackFnParams) {

return (int)wParam;
}

void _plGetClientRect(_plHWND hwnd, XRect &rc){

GetClientRect(hwnd, &rc);
}

void _plMoveWindow(_plHWND hwnd, int x, int y, int width, int height){

MoveWindow(hwnd, x, y, width, height, FALSE);
}

void _plShowWindow(_plHWND hwnd, bool show){

ShowWindow(hwnd, (!show && SW_HIDE) || (show && SW_SHOW));
}

int _plGetX(_plCallbackFnParams){

return LOWORD(lParam);
}

int _plGetY(_plCallbackFnParams){

return HIWORD(lParam);
}

int _plGetMouseVKey(_plCallbackFnParams){

return GET_KEYSTATE_WPARAM(wParam);
}

Tchar _plGetChar(_plCallbackFnParams){

return (Tchar) wParam;
}

int _plGetKey(_plCallbackFnParams){

return (Tchar) wParam;
}

int _plGetVKeyState(int virtual_key){

return GetKeyState(virtual_key);
}

int _plGetMouseWheelDelta(_plCallbackFnParams){

return GET_WHEEL_DELTA_WPARAM(wParam);
}

_plHDC _plGetHDC(_plHWND hwnd, _plContextParams *params){

return BeginPaint(hwnd, params);
}

void _plReleaseHDC(_plHWND hwnd, _plContextParams *params){

EndPaint(hwnd, params);
}

_plHDC _plGetStandAloneHDC(_plHWND hwnd){

return GetDC(hwnd);
}

void _plReleaseStandAloneHDC(_plHWND hwnd, _plHDC dc){

ReleaseDC(hwnd, dc);
}

_plCOLOR _plGetColorRGB(unsigned char red, unsigned char green, unsigned char blue){

return RGB(red, green, blue);
}

void _plTextOut(_plHDC dc, int x, int y, const Tchar *text, const size_t len){

TextOut(dc, x, y, text, (int)len);
}

void _plTextOut(_plHDC dc, int x, int y, int options, XRect &rc, const Tchar *text, const size_t len){

ExtTextOut(dc, x, y, options, &rc, text, (int)len, 0);
}

void _plRectangle(_plHDC dc, int left, int top, int right, int bottom){

Rectangle(dc ,left, top, right, bottom);
}

void _plPolygon(_plHDC dc, const XPoint *points, const size_t count){

Polygon(dc, points, (int)count);
}

_plHPEN _plCreatePen(int style, int width, unsigned char red, unsigned char green, unsigned char blue){

return CreatePen(style, width, RGB(red, green, blue));
}

_plHPEN _plCreateSpecialPen(const int pen_id){

return (_plHPEN)GetStockObject(pen_id);
}

_plHBRUSH _plCreateBrush(unsigned char red, unsigned char green, unsigned char blue){

return CreateSolidBrush(RGB(red, green, blue));
}

_plHBRUSH _plCreateSpecialBrush(const int brush_id){

return (_plHBRUSH)GetStockObject(brush_id);
}

_plGCObject _plSelectObject(_plHDC dc, _plGCObject obj){

return SelectObject(dc, obj);
}

void _plReleaseObject(_plGCObject obj){

DeleteObject(obj);
}

int _plSetBackgroundFillMode(_plHDC dc, int mode){

return SetBkMode(dc, mode);
}

_plCOLOR _plSetBackgroundColor(_plHDC dc, _plCOLOR color){

return SetBkColor(dc, color);
}

int _plGetScrollThumbPos(_plCallbackFnParams){

return HIWORD(wParam);
}

int _plGetScrollThumbPosLarge(_plCallbackFnParams) {
_plSCROLLINFO si;

si.cbSize = sizeof(_plSCROLLINFO);
si.fMask = SIF_TRACKPOS;
si.nMin = 0;
si.nMax = 0;
si.nPage = 0;
si.nPos = 0;
si.nTrackPos = 0;
	
int scroll_orient = (message == WM_VSCROLL) ? SB_VERT : SB_HORZ;
GetScrollInfo(hWnd, scroll_orient, &si);

return si.nTrackPos;
}

int _plGetScrollType(_plCallbackFnParams){

return LOWORD(wParam);
}

void _plSetScrollInfo(_plHWND hwnd, const int scroll_orient, const int max,\
				   const int page, const int pos){
_plSCROLLINFO si;

si.cbSize = sizeof(_plSCROLLINFO);
si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
si.nMin = 0; 
si.nMax = max; 
si.nPage = page; 
si.nPos = pos; 
SetScrollInfo(hwnd, scroll_orient, &si, TRUE); 
}

void _plSetScrollInfo(_plHWND hwnd, const int scroll_orient, \
						const XScrollParams &params) {

_plSetScrollInfo(hwnd, scroll_orient, \
					params.max, params.page, params.pos);
}

int _plGetScrollPos(_plHWND hwnd, const int scroll_orient){

return GetScrollPos(hwnd, scroll_orient);
}

void _plComboBoxAddItem(_plHWND hwnd, const Tchar* item){

SendMessage(hwnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) item);
}

void _plComboBoxDelItem(_plHWND hwnd, const size_t index) {

SendMessage(hwnd, (UINT) CB_DELETESTRING, (WPARAM)index, (LPARAM)0);
}

size_t _plComboBoxGetCurrSel(_plHWND hwnd){

// returns -1 if no selection made
return SendMessage(hwnd, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
}

size_t _plComboBoxGetItemTextLen(_plHWND hwnd, const size_t index){

return SendMessage(hwnd, (UINT) CB_GETLBTEXTLEN, \
                (WPARAM) index, (LPARAM) 0);
}

void _plComboBoxSetSel(_plHWND hwnd, const size_t index) {

SendMessage(hwnd, (UINT)CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
}

void _plComboBoxGetItemText(_plHWND hwnd, const size_t index,\
							Tchar *text){

SendMessage(hwnd, (UINT) CB_GETLBTEXT, \
                (WPARAM) index, (LPARAM) text);
}

void _plComboBoxReset(_plHWND hwnd){

SendMessage(hwnd, (UINT) CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
}

void _plMessageBox(const Tchar *message){

MessageBox(0, message, _T("Info"), MB_OK);
}

void _plMessageBoxANSI(const char *message){

MessageBoxA(0, message, "Info", MB_OK);
}

void ErrorBox(const Tchar *message) {

MessageBox(0, message, _T("Error"), MB_OK | MB_ICONERROR);
}

void ErrorBoxANSI(const char *message) {

MessageBoxA(0, message, "Error", MB_OK | MB_ICONERROR);
}

_plHFONT _plCreateFont(int size, int rotation_angle, int symbols_angle, \
					int properties, _plFONT_CHARSET charset, const Tchar *lpszFace){

return CreateFont(size, 0, rotation_angle, symbols_angle, 400 + 300*(properties & X_BOLD),\
				 properties & X_ITALIC, properties & X_UNDERLINE, properties & X_STRIKEOUT,\
				charset, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 0, lpszFace);
}

const Tchar *GetExecutableFullPath(Tchar *buffer) {

if (GetModuleFileName(0, buffer, MAX_PATH + 1))
	return buffer;
else
	return nullptr;
}
