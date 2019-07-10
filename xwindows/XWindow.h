#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include <basic/Exception.h>
#include "XEventHandler.h"

class XWindowException : public XException{
public:
	enum Errors{
		E_PARENT_WINDOW = 1,\
		E_REGISTER_WINDOW = 2,\
		E_CREATE_WINDOW = 3,\
		E_WINDOW_OBJ = 4,\
		E_WINDOW_ALREADY_CREATED = 5, \
		E_WINDOW_CLASS = 6
	};
	enum Warnings{
		W_CHILD_WINDOW_ATTACHED = RESULT_FAIL
	};
	XWindowException(const int err_code, const Tchar *err_descr);
	XWindowException(const XWindowException &obj);
	XWindowException(XWindowException &&obj) = default;
	~XWindowException();
};

class XWindow : public XEventHandler{

	struct CChildWindow{
		int id;
		XWindow *window;
		CChildWindow() noexcept : id(-1), window(nullptr){ }
		CChildWindow(const CChildWindow &obj) noexcept = default;
		CChildWindow(CChildWindow &&obj) noexcept = default;
		CChildWindow &operator=(const CChildWindow &obj) noexcept = default;
		CChildWindow &operator=(CChildWindow &&obj) noexcept = default;
		inline bool operator<(const CChildWindow &obj) const{
			
			return id < obj.id;
		}
		inline bool operator==(const CChildWindow &obj) const{
			
			return id == obj.id;
		}
	};
	std::vector<CChildWindow> child_windows;
	bool removed_from_children_registry;
	
	bool is_created;
	void OnWindowCreationCompleted() { is_created = true; }
	XWindow *GetThisAsXWindow() { return this; }

	XWindow *parent;
	Tstring class_name;
	std::vector<Tchar> label;
	int creation_flags;

	int tag;

	int AddChild(const int id, XWindow *child_window);
	void RemoveChild(const int id);
	void DestroyAllChildren();
protected:
	inline void CheckAndInitWindowClassName(const Tchar *class_name);
	template <typename RegisterFunc> \
		inline void CheckAndInitWindowClassName(const Tchar *class_name, \
							RegisterFunc register_func);
	inline static void CheckParent(XWindow *parent, const int id, \
					const Tchar *label, const Tchar *class_name);

	template <typename CreationFunc> \
		inline void CreateImpl(XWindow *parent, const Tchar *class_name, \
								const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height, \
								CreationFunc creation_func);

	inline void SetScrollBar(const int scroll_orient, const int max,\
								const int page, const int pos);
	inline void SetScrollBar(const int scroll_orient, \
								const XScrollParams &params);
	inline int GetScrollPos(const int scroll_orient);

	bool send_destroy_event;
	void Destroy();
public:
	XWindow() noexcept;
	XWindow(const XWindow &obj) = delete;
	XWindow(XWindow &&obj);
	XWindow &operator=(const XWindow &obj) = delete;
	XWindow &operator=(XWindow &&obj);

	inline XWindow *GetChildById(const int id) const;
	inline bool IsCreated() const{ return is_created; }
	inline XWindow *GetParent(){ return parent; }
	inline const XWindow *GetParent() const { return parent; }
	inline int GetTag() const { return tag; }
	inline void SetTag(const int tag) { this->tag = tag; }
	inline const Tstring &GetClassName() const{ return class_name; }
	inline const Tchar *GetLabel();
	inline const Tchar *GetLabel(size_t &size);
	inline const Tchar *GetLabel(std::vector<Tchar> &label) const;
	inline bool SetLabel(const Tchar *label);
	inline bool SetLabel(const Tchar *label, const size_t size);
	inline void GetClientRect(XRect &client_rect) const;
	inline void MoveWindow(const int x, const int y, const int width, const int height);
	inline void SetFocus();
	inline void Invalidate(const XRect *rect, const bool redraw);
	inline void Enable() { _plEnableWindow(GetInternalHandle(), true); }
	inline void Disable() { _plEnableWindow(GetInternalHandle(), false); }
	inline void Show(){ _plShowWindow(GetInternalHandle(), true); }
	inline void Hide(){ _plShowWindow(GetInternalHandle(), false); }
	virtual ~XWindow();
};

//*****************************************************************

void XWindow::CheckAndInitWindowClassName(const Tchar *class_name) {

if (!this->class_name.empty()) {
	XWindowException e(XWindowException::E_REGISTER_WINDOW, \
		_T("An attempt to re-register the window class '"));
	e << class_name << _T("' again. The present Class_name = ");
	e << this->class_name;
	throw e;
}
this->class_name = class_name;
}

template <typename RegisterFunc> \
void XWindow::CheckAndInitWindowClassName(const Tchar *class_name, \
											RegisterFunc register_func) {

CheckAndInitWindowClassName(class_name);
if (register_func()) this->class_name = class_name;
}

void XWindow::CheckParent(XWindow *parent, const int id, \
							const Tchar *label, const Tchar *class_name){
auto appendWindowParamsToException = [id, label, class_name](XException &e){

	e << _T(" ID = ") << id << _T(", Label = '") << label << _T("'");
	if (class_name) e << _T(", Class = '") << class_name << _T("'");
};

if (!parent) {
	XWindowException e(XWindowException::E_PARENT_WINDOW, \
		_T("the passed parent window object poiner is NULL for the window:"));
	appendWindowParamsToException(e);
	throw e;
}
if (!parent->IsCreated()) {
	XWindowException e(XWindowException::E_WINDOW_OBJ, \
		_T("the passed parent window object does not yet represent a window. The passed window params are: "));
	appendWindowParamsToException(e);
	throw e;
}
}

template <typename CreationFunc> \
void XWindow::CreateImpl(XWindow *parent, const Tchar *class_name, \
							const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height, \
							CreationFunc creation_func) {
const Tchar *Label = label ? label : _T("");
int id = InitializeId();

assert(id != -1 && (!parent && !id || parent && id));
assert(id == GetId());

if (!class_name || (class_name && *class_name == _T('\0'))) {
	XWindowException e(XWindowException::E_WINDOW_CLASS, \
		_T("the passed window class name is empty:"));
	e << _T(" ID = ") << id << _T(", Label = '") << Label << _T("'");
	throw e;
}

if (is_created) {
	XWindowException e(XWindowException::E_WINDOW_ALREADY_CREATED, \
		_T("the window object with such ID is already created:"));
	e << _T(" ID = ") << id << _T(", Label = '") << Label << _T("'");
	e << _T(", Class = '") << class_name << _T("'");
	throw e;
}

this->parent = parent;

this->label.resize(Tstrlen(Label) + 1);
Tstrcpy(&this->label[0], Label);
this->creation_flags = flags;

if (parent) {
	int ret_value = parent->AddChild(id, this);
	assert(ret_value != XWindowException::W_CHILD_WINDOW_ATTACHED);
}

is_created = false;
_plHWND hwnd;
hwnd = creation_func(parent ? parent->GetInternalHandle() : 0, class_name, \
						flags, x, y, width, height, Label, id, this);
if (!hwnd) {
	XWindowException e(XWindowException::E_CREATE_WINDOW, \
						_T("cannot create the window: "));
	if(parent){
		parent->RemoveChild(id);
		e << _T("Parent_label = '") << parent->GetLabel()<< _T("', ");
	}
	e << _T("Class = '") << class_name;
	e << _T("', ID = ") << id << _T(", Label = '") << Label << _T("'");
	throw e;
}

SetInternalHandle(hwnd);
is_created = true;
}

XWindow *XWindow::GetChildById(const int id) const{
CChildWindow rec;
std::vector<CChildWindow>::const_iterator p;

rec.id = id;
p = std::lower_bound(child_windows.cbegin(), child_windows.cend(), rec);
if(p != child_windows.cend() && rec == *p)
	return p->window;

return nullptr;
}

void XWindow::GetClientRect(XRect &client_rect) const{
	
_plGetClientRect(GetInternalHandle(), client_rect);
}

void XWindow::SetFocus(){

_plSetFocus(GetInternalHandle());
}

void XWindow::Invalidate(const XRect *rect, const bool redraw){

_plInvalidate(GetInternalHandle(), rect, redraw);
}

void XWindow::MoveWindow(const int x, const int y, \
						 const int width, const int height){

_plMoveWindow(GetInternalHandle(), x, y, width, height);
}

const Tchar *XWindow::GetLabel(){

	return GetLabel(this->label);
}

const Tchar *XWindow::GetLabel(size_t &size){
	const Tchar *p_label = GetLabel();

size = label.empty() ? 0 : label.size() - 1;

return p_label;
}

const Tchar *XWindow::GetLabel(std::vector<Tchar> &label) const {

	size_t label_size = _plGetLabelSize(GetInternalHandle());
	label.resize(label_size + 1);

	if (label_size > 0)
		_plGetLabel(GetInternalHandle(), &label[0], label_size + 1);
	label[label_size] = _T('\0');

	return &label[0];
}

bool XWindow::SetLabel(const Tchar *label){

assert(label);
if(_plSetLabel(GetInternalHandle(), label)){
	this->label.resize(Tstrlen(label) + 1);
	Tstrcpy(&this->label[0], label);
	return true;
}
return false;
}

bool XWindow::SetLabel(const Tchar *label, const size_t size){

assert(label);
if(_plSetLabel(GetInternalHandle(), label)){
	this->label.resize(size + 1);
	Tstrncpy(&this->label[0], label, size);
	this->label[size] = '\0';
	return true;
}
return false;
}

void XWindow::SetScrollBar(const int scroll_orient, const int max,\
						   const int page, const int pos){

_plSetScrollInfo(GetInternalHandle(), scroll_orient, max, page, pos);
}

void XWindow::SetScrollBar(const int scroll_orient, \
							const XScrollParams &params) {

_plSetScrollInfo(GetInternalHandle(), scroll_orient, params);
}

int XWindow::GetScrollPos(const int scroll_orient){

return _plGetScrollPos(GetInternalHandle(), scroll_orient);
}
