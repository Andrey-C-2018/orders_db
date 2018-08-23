#include <assert.h>
#include "XGC.h"

XGC::XGC() noexcept : left_bound(0), upper_bound(0){

_plUnsetHWND(hwnd);
_plUnsetHDC(dc);
}

XGC::XGC(XGC &&obj) noexcept : left_bound(obj.left_bound), \
								upper_bound(obj.upper_bound), \
								hwnd(obj.hwnd), dc(obj.dc){

obj.left_bound = 0;
obj.upper_bound = 0;
_plUnsetHWND(obj.hwnd);
_plUnsetHDC(obj.dc);
}

XGC &XGC::operator=(XGC &&obj) noexcept {
	
	left_bound = obj.left_bound;
	upper_bound = obj.upper_bound;
	hwnd = obj.hwnd;
	dc = obj.dc;

	obj.left_bound = 0;
	obj.upper_bound = 0;
	_plUnsetHWND(obj.hwnd);
	_plUnsetHDC(obj.dc);
	return *this;
}

void XGC::DisplayText(int X, int Y, const Tchar *text, const size_t len) noexcept {

_plTextOut(dc, X, Y, text, len);
}

void XGC::DisplayText(int X, int Y, int options, XRect &rect,\
				  const Tchar *text, const size_t len) noexcept {

_plTextOut(dc, X, Y, options, rect, text, len);
}

void XGC::DisplayTextWithBounds(int X, int Y, int options, const XRect &rect, \
								const Tchar *text, const size_t len) noexcept {
	XRect new_rect;

	new_rect.left = LimitToLowerBound(rect.left, left_bound);
	new_rect.top = LimitToLowerBound(rect.top, upper_bound);
	new_rect.right = LimitToLowerBound(rect.right, left_bound);
	new_rect.bottom = LimitToLowerBound(rect.bottom, upper_bound);

	_plTextOut(dc, X, Y, options, new_rect, text, len);
}

void XGC::Rectangle(int left, int top, int right, int bottom) noexcept {

_plRectangle(dc, left, top, right, bottom);
}

void XGC::RectangleWithBounds(int left, int top, int right, int bottom) noexcept {
	
_plRectangle(dc, LimitToLowerBound(left, left_bound), \
					LimitToLowerBound(top, upper_bound), \
					LimitToLowerBound(right, left_bound), \
					LimitToLowerBound(bottom, upper_bound));
}

void XGC::Polygon(const XPoint *points, const size_t count) noexcept {

_plPolygon(dc, points, count);
}

int XGC::SetBackgroundFillMode(const int mode) noexcept {

return _plSetBackgroundFillMode(dc, mode);
}

XColor XGC::SetBackgroundColor(const XColor &color) noexcept {

return XColor(_plSetBackgroundColor(dc, color.GetInternalHandle()));
}

XGC::~XGC(){ }

//*************************************************************

XPaintEventGC::XPaintEventGC(XWindow *wnd) noexcept {

	assert(wnd);
	memset(&context_params, 0, sizeof(_plContextParams));
	hwnd = wnd->GetInternalHandle();
	dc = _plGetHDC(hwnd, &context_params);
}

XPaintEventGC::XPaintEventGC(XPaintEventGC &&obj) noexcept : XGC(std::move(obj)) {

	memcpy(&context_params, &obj.context_params, sizeof(_plContextParams));
	memset(&obj.context_params, 0, sizeof(_plContextParams));
}

XPaintEventGC &XPaintEventGC::operator=(XPaintEventGC &&obj) noexcept {

	XGC::operator=(std::move(obj));
	memcpy(&context_params, &obj.context_params, sizeof(_plContextParams));
	memset(&obj.context_params, 0, sizeof(_plContextParams));
	return *this;
}

XPaintEventGC::~XPaintEventGC() {

	if (_plIsHWNDSet(hwnd) && _plIsHDCSet(dc))
		_plReleaseHDC(hwnd, &context_params);
}

//*************************************************************

XStandAloneGC::XStandAloneGC(XWindow *wnd) noexcept {

assert(wnd);
hwnd = wnd->GetInternalHandle();
dc = _plGetStandAloneHDC(hwnd);
}

XStandAloneGC::~XStandAloneGC(){

if (_plIsHWNDSet(hwnd) && _plIsHDCSet(dc)) {
	_plReleaseStandAloneHDC(hwnd, dc);
	_plUnsetHWND(hwnd);
	_plUnsetHDC(dc);
}
}
