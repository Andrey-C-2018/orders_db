#include <functional>
#include "XWindow.h"

XWindowException::XWindowException(const int err_code, const Tchar *err_descr) : \
				XException(err_code, err_descr){ }

XWindowException::XWindowException(const XWindowException &obj) : \
				XException(obj){ }

XWindowException::~XWindowException(){ }

//****************************************************************************

XWindow::XWindow() noexcept : removed_from_children_registry(false), send_destroy_event(true), tag(0), \
								is_created(false), parent(nullptr), creation_flags(0){ }

XWindow::XWindow(XWindow &&obj) : child_windows(std::move(obj.child_windows)),\
						removed_from_children_registry(obj.removed_from_children_registry), tag(obj.tag), \
						send_destroy_event(obj.send_destroy_event), \
						is_created(obj.is_created), parent(obj.parent), \
						class_name(std::move(obj.class_name)), \
						label(std::move(obj.label)), \
						creation_flags(obj.creation_flags){
obj.removed_from_children_registry = true;
obj.send_destroy_event = false;
obj.tag = 0;
obj.is_created = false;
obj.parent = nullptr;
obj.creation_flags = 0;
}

XWindow &XWindow::operator=(XWindow &&obj) {

child_windows = std::move(obj.child_windows);
removed_from_children_registry = obj.removed_from_children_registry;
tag = obj.tag;
send_destroy_event = obj.send_destroy_event;
is_created = obj.is_created;
parent = obj.parent;
class_name = std::move(obj.class_name);
label = std::move(obj.label);
creation_flags = obj.creation_flags;

obj.removed_from_children_registry = true;
obj.send_destroy_event = false;
obj.tag = 0;
obj.is_created = false;
obj.parent = nullptr;
obj.creation_flags = 0;
return *this;
}

int XWindow::AddChild(const int id, XWindow *child_window){
CChildWindow rec;
std::vector<CChildWindow>::iterator p;

assert(child_window);
rec.id = id;
rec.window = child_window;
p = std::lower_bound(child_windows.begin(), child_windows.end(), rec);
if(!(p != child_windows.end() && rec == *p)){
	child_windows.push_back(rec);
	std::sort(child_windows.begin(), child_windows.end());
	return RESULT_SUCCESS;
}

return XWindowException::W_CHILD_WINDOW_ATTACHED;
}

void XWindow::RemoveChild(const int id) {
CChildWindow rec;
std::vector<CChildWindow>::iterator p;

rec.id = id;
p = std::lower_bound(child_windows.begin(), child_windows.end(), rec);

if (p != child_windows.cend() && rec == *p)
	child_windows.erase(p);
}

void XWindow::DestroyAllChildren(){

std::vector<CChildWindow>::iterator p;
for (p = child_windows.begin(); p != child_windows.end(); ++p) {
	if (p->window) {
		p->window->removed_from_children_registry = true;
		delete p->window;
		p->window = nullptr;
	}
}
child_windows.clear();
}

void XWindow::Destroy() {

if (is_created) {
	DestroyAllChildren();
	if (parent && !removed_from_children_registry)
		parent->RemoveChild(GetId());

	is_created = false;
	if (send_destroy_event) {
		send_destroy_event = false;
		_plDestroyWindow(GetInternalHandle());
	}
}
}

XWindow::~XWindow(){

Destroy();
}
