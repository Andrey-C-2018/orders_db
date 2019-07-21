#pragma once
#include <basic/Exception.h>
#include "XWindow.h"

class XWidget : public XWindow{
	class CRegisterPredicate {
		XWidget &widget;
		const Tchar *class_name;
	public:
		CRegisterPredicate(XWidget &widget_, \
							const Tchar *class_name_) noexcept: \
							widget(widget_), class_name(class_name_){ }

		inline bool operator()() {
			
			if (!_plRegisterWidget(class_name, widget.MainWndProc)) {

				XWindowException e(XWindowException::E_REGISTER_WINDOW, \
									_T("cannot register the widget class:"));
				e << _T(" Class_name = ") << class_name;
				throw e;
			}
			return true;
		}
	};

protected:
	int InitializeId() override { return CreateUniqueId(); }
	inline void RegisterNewWidgetClass(const Tchar *class_name);

public:
	XWidget() noexcept : XWindow() { }

	XWidget(const XWidget &obj) = delete;
	XWidget(XWidget &&obj) = default;
	XWidget &operator=(const XWidget &obj) = delete;
	XWidget &operator=(XWidget &&obj) = default;
	
	XWidget(XWindow *parent, const int flags, \
							const Tchar *label,\
							const int x, const int y,\
							const int width, const int height);

	virtual void Create(XWindow *parent, const int flags, \
							const Tchar *label,\
							const int x, const int y,\
							const int width, const int height);
	virtual ~XWidget(){ }
};

//****************************************************************************

void XWidget::RegisterNewWidgetClass(const Tchar *class_name){

	CheckAndInitWindowClassName(class_name, CRegisterPredicate(*this, class_name));
}
