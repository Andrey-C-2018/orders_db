#pragma once
#include "XWindow.h"

template <typename TObject> class XGC_Object{
	bool initialized;
	TObject object;
protected:
	inline void Initialize(_plGCObject object) noexcept{

		this->object = (TObject)object; 
		initialized = true;
	}
public:
	XGC_Object() noexcept : initialized(false){ }

	inline XGC_Object(_plGCObject object) noexcept : initialized(false){

		Initialize(object);
	}

	XGC_Object(const XGC_Object &obj) = delete;
	XGC_Object(XGC_Object &&obj) noexcept : initialized(obj.initialized), \
											object(obj.object){
		obj.initialized = false;
	}
	XGC_Object &operator=(const XGC_Object &obj) = delete;
	inline XGC_Object &operator=(XGC_Object &&obj) noexcept {

		Initialize(obj.object);
		obj.initialized = false;
		return *this;
	}

	inline TObject GetInternalHandle() const{ return object; }
	inline bool IsNull() const{ return initialized == false; } 

	inline void Reset() {

		initialized = false;
	}

	inline void Destroy(){

		if(initialized) _plReleaseObject(object);
	}

	virtual ~XGC_Object(){ }
};

class XPen : public XGC_Object<_plHPEN>{
	bool is_special;
public:
	XPen() noexcept : is_special(false){ }
	XPen(const XPen &obj) = delete;
	XPen(XPen &&obj) noexcept : XGC_Object<_plHPEN>(std::move(obj)), \
								is_special(obj.is_special){
		obj.is_special = true;
	}
	XPen &operator=(const XPen &obj) = delete;
	XPen &operator=(XPen &&obj) noexcept {
		XGC_Object<_plHPEN>::operator=(std::move(obj));
		is_special = obj.is_special;
		obj.is_special = true;
		return *this;
	}

	inline XPen(_plGCObject object) noexcept : is_special(true){

		Initialize(object);
	}
	
	XPen(int style, int width,\
		unsigned char red, unsigned char green, unsigned char blue) noexcept : \
																is_special(false){
		
		Initialize(_plCreatePen(style, width, red, green, blue));
	}

	XPen(const int special_pen) noexcept : is_special(true){
	
		Initialize(_plCreateSpecialPen(special_pen));
	}

	void Destroy() noexcept {

		if(!is_special){
			XGC_Object<_plHPEN>::Destroy();
			is_special = true;
		}
	}

	virtual ~XPen(){
	
		Destroy();
	}
};

class XBrush : public XGC_Object<_plHBRUSH>{
	bool is_special;
public:
	XBrush() noexcept : is_special(false){ }
	XBrush(const XBrush &obj) = delete;
	XBrush(XBrush &&obj) noexcept : XGC_Object<_plHBRUSH>(std::move(obj)), \
									is_special(obj.is_special) {
		obj.is_special = true;
	}
	XBrush &operator=(const XBrush &obj) = delete;
	XBrush &operator=(XBrush &&obj) noexcept {

		XGC_Object<_plHBRUSH>::operator=(std::move(obj));
		is_special = obj.is_special;
		obj.is_special = true;
		return *this;
	}

	inline XBrush(_plGCObject object) noexcept : is_special(true){

		Initialize(object);
	}
	
	XBrush(unsigned char red, unsigned char green, unsigned char blue) noexcept : \
																	is_special(false){
		
		Initialize(_plCreateBrush(red, green, blue));
	}

	XBrush(const int special_brush) noexcept : is_special(true){
	
		Initialize(_plCreateSpecialBrush(special_brush));
	}

	void Destroy() noexcept {

		if(!is_special){
			XGC_Object<_plHBRUSH>::Destroy();
			is_special = true;
		}
	}

	virtual ~XBrush(){
	
		Destroy();
	}
};

class XFont : public XGC_Object<_plHFONT>{
	bool is_special;
public:
	XFont() noexcept : is_special(false){ }
	XFont(const XFont &obj) = delete;
	XFont(XFont &&obj) noexcept : XGC_Object<_plHFONT>(std::move(obj)), \
									is_special(obj.is_special) {
		obj.is_special = true;
	}
	XFont &operator=(const XFont &obj) = delete;
	XFont &operator=(XFont &&obj) noexcept {
		XGC_Object<_plHFONT>::operator=(std::move(obj));
		is_special = obj.is_special;
		obj.is_special = true;
		return *this;
	}

	inline XFont(_plGCObject object) noexcept : is_special(true){

		Initialize(object);
	}
	XFont(int size, int rotation_angle, int symbols_angle, \
		int properties, _plFONT_CHARSET charset, const Tchar *lpszFace) noexcept : \
												is_special(false){

		Initialize(_plCreateFont(size, rotation_angle, \
					symbols_angle, properties, charset, lpszFace));
	}
	void Destroy() noexcept {

		if(!is_special){
			XGC_Object<_plHFONT>::Destroy();
			is_special = true;
		}
	}
	virtual ~XFont(){
	
		Destroy();
	}
};

class XColor{
	_plCOLOR color;
	bool initialized;
public:
	inline XColor() noexcept : color(0), initialized(false){ }
	inline XColor(const _plCOLOR _color) noexcept : \
					color(_color), initialized(true){ }
	inline XColor(unsigned char red, \
					unsigned char green, \
					unsigned char blue) noexcept {

		color = _plGetColorRGB(red, green, blue);
		initialized = true;
	}
	inline _plCOLOR GetInternalHandle() const{ return color; }
	inline bool IsNull() const { return !initialized; }
	inline void Reset() { initialized = false; }
};

class XGC{
	int left_bound, upper_bound;

	XGC(const XGC &obj) = delete;
	XGC &operator=(const XGC &obj) = delete;
protected:
	_plHWND hwnd;
	_plHDC dc;

	XGC() noexcept;
	inline int LimitToLowerBound(const int value, const int lower_bound) const{
		int bigger = (value >= lower_bound);
		return  bigger * value + !bigger * lower_bound;
	};
public:
	XGC(XGC &&obj) noexcept;
	XGC &operator=(XGC &&obj) noexcept;

	inline _plHDC GetInternalHandle() const{ return dc; }

	inline void SetBounds(int left_bound, int upper_bound) {

		this->left_bound = left_bound;
		this->upper_bound = upper_bound;
	}

	void DisplayText(int X, int Y, const Tchar *text, const size_t len) noexcept;
	void DisplayText(int X, int Y, int options, XRect &rect,\
					const Tchar *text, const size_t len) noexcept;
	void DisplayTextWithBounds(int X, int Y, int options, const XRect &rect, \
					const Tchar *text, const size_t len) noexcept;
	void Rectangle(int left, int top, int right, int bottom) noexcept;
	void RectangleWithBounds(int left, int top, int right, int bottom) noexcept;
	void Polygon(const XPoint *points, const size_t count) noexcept;

	template <class TGC_Object> \
		TGC_Object SelectObject(const TGC_Object &object) noexcept {
			_plGCObject old_object;

			assert(!object.IsNull());
			old_object = _plSelectObject(dc, object.GetInternalHandle());

			return TGC_Object(old_object);
		}

	int SetBackgroundFillMode(const int mode) noexcept;
	XColor SetBackgroundColor(const XColor &color) noexcept;

	virtual ~XGC();
};

class XPaintEventGC : public XGC {
	_plContextParams context_params;

	XPaintEventGC(const XPaintEventGC &obj) = delete;
	XPaintEventGC &operator=(const XPaintEventGC &obj) = delete;
public:
	XPaintEventGC(XWindow *wnd) noexcept;

	XPaintEventGC(XPaintEventGC &&obj) noexcept;
	XPaintEventGC &operator=(XPaintEventGC &&obj) noexcept;
	virtual ~XPaintEventGC();
};

class XStandAloneGC : public XGC{
	XStandAloneGC(const XStandAloneGC &obj) = delete;
	XStandAloneGC &operator=(const XStandAloneGC &obj) = delete;
public:
	XStandAloneGC(XWindow *wnd) noexcept;

	XStandAloneGC(XStandAloneGC &&obj) noexcept : XGC(std::move(obj)) { }
	XStandAloneGC &operator=(XStandAloneGC &&obj) noexcept {
		XGC::operator=(std::move(obj));
		return *this;
	}

	virtual ~XStandAloneGC();
};

class XTextMetric : public _plXTextMetric{
public:
	XTextMetric(const XGC *gc) noexcept : _plXTextMetric(gc->GetInternalHandle()){ }
};
