#include <xwindows/XWidget.h>
#include "XTabStopManager.h"

XTabStopManager::XTabStopManager(XWindow *tabstop_parent_) : \
								tabstop_parent(tabstop_parent_) {

	assert(tabstop_parent);
	tabstop_widgets.reserve(DEF_TABSTOP_WIDGETS_COUNT);
}

void XTabStopManager::RegisterTabStopControl(XWidget *widget) {

	assert(widget);
	CTabStopItem item;
	item.widget = widget;
	item.tabstop_action = nullptr;
	tabstop_widgets.push_back(item);
}

void XTabStopManager::RegisterTabStopControl(XWidget *widget, ITabStopAction *tabstop_action) {

	assert(widget);
	CTabStopItem item;
	item.widget = widget;
	item.tabstop_action = tabstop_action;
	tabstop_widgets.push_back(item);
}

void XTabStopManager::TabPressedOnControl(XWidget *widget) {

	assert(widget);
	assert(tabstop_parent == widget->GetParent());

	CTabStopItem item;
	item.widget = widget;
	auto p = std::find(tabstop_widgets.begin(), tabstop_widgets.end(), item);
	if (p == tabstop_widgets.end()) {
		XException e(0, _T("the widget was not registered as tabstop: class name = "));
		e << widget->GetClassName() << _T(", ID = ") << widget->GetId();
		throw e;
	}

	if (p + 1 != tabstop_widgets.end()) {
		++p;
		XWindow *next_widget = tabstop_parent->GetChildById(p->widget->GetId());
		assert(next_widget);
		next_widget->SetFocus();

		if (p->tabstop_action) p->tabstop_action->exec(p->widget);
	}
}

XTabStopManager::~XTabStopManager() { }
