#include "XWidget.h"

XWidget::XWidget(XWindow *parent, const int flags, \
						const Tchar *label,\
						const int x, const int y,\
						const int width, const int height){

	Create(parent, flags, label, x, y, width, height);
}

void XWidget::Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height){

	CheckParent(parent, GetId(), label, GetClassName().c_str());
	CreateImpl(parent, GetClassName().c_str(), flags, label, \
					x, y, width, height, \
					[](_plHWND hwnd_parent, const Tchar *class_name, \
						const int flags, const int X, const int Y, \
						const int width, const int height, \
						const Tchar *label, const int id, \
						XWindow *this_window) -> _plHWND {

						return _plCreateWidget(hwnd_parent, class_name, \
										flags, X, Y, width, height, \
										label, id, this_window);
					});
}

