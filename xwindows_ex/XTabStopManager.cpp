#include <xwindows/XWidget.h>
#include "XTabStopManager.h"

XTabStopManager::XTabStopManager(XWindow *tabstop_parent_) : \
								tabstop_parent(tabstop_parent_) {

	assert(tabstop_parent);
}

void XTabStopManager::RegisterTabStopControl(XWidget *widget) {

	assert(widget);
	tabstop_widgets.reserve(tabstop_parent->GetChildrenCount() + 1);
	tabstop_widgets.push_back(widget);
}

void XTabStopManager::TabPressedOnControl(XWidget *widget) {

	assert(widget);
	assert(tabstop_parent == widget->GetParent());

	auto p = std::find(tabstop_widgets.cbegin(), tabstop_widgets.cend(), widget);
	if (p == tabstop_widgets.cend()) {
		XException e(0, _T("the widget was not registered as tabstop: class name = "));
		e << widget->GetClassNameW() << _T(", ID = ") << widget->GetId();
		throw e;
	}

	if (p + 1 != tabstop_widgets.cend()) {
		++p;
		XWindow *next_widget = tabstop_parent->GetChildById((*p)->GetId());
		assert(next_widget);
		next_widget->SetFocus();
	}
}

XTabStopManager::~XTabStopManager() { }
