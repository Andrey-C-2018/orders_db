#pragma once
#include <xwindows/XWidget.h>
#include "ITabStopManager.h"

template <class TWidget> \
class XTabStopWidget : public TWidget {
	ITabStopManager *manager;

	void OnKeyPress(XKeyboardEvent *eve);
public:
	XTabStopWidget(ITabStopManager *manager_) noexcept;

	XTabStopWidget(ITabStopManager *manager_, XWindow *parent, \
						const int flags, const Tchar *label, \
						const int x, const int y, \
						const int width, const int height);

	XTabStopWidget(XTabStopWidget &&obj) = default;
	XTabStopWidget(const XTabStopWidget &obj) = delete;
	XTabStopWidget &operator=(const XTabStopWidget &obj) = delete;
	XTabStopWidget &operator=(XTabStopWidget &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	virtual ~XTabStopWidget();
};

//*****************************************************

template <class TWidget> \
XTabStopWidget<TWidget>::XTabStopWidget(ITabStopManager *manager_) noexcept : \
									manager(manager_) { }

template <class TWidget> \
XTabStopWidget<TWidget>::XTabStopWidget(ITabStopManager *manager_, XWindow *parent, \
										const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) : \
									XEdit(parent, flags, label, x, y, width, height), \
									manager(manager_) {

	assert(manager);
	manager->RegisterTabStopControl(this);

	OverrideWindowEvent(EVT_KEYDOWN, this, &XTabStopEdit::OnKeyPress);
}

template <class TWidget> \
void XTabStopWidget<TWidget>::Create(XWindow *parent, const int flags, \
									const Tchar *label, \
									const int x, const int y, \
									const int width, const int height) {

	assert(manager);
	manager->RegisterTabStopControl(this);

	TWidget::Create(parent, flags, label, x, y, width, height);
	OverrideWindowEvent(EVT_KEYDOWN, XEventHandlerData(this, &XTabStopWidget::OnKeyPress));
}

template <class TWidget> \
void XTabStopWidget<TWidget>::OnKeyPress(XKeyboardEvent *eve) {

	if (eve->GetKey() != X_VKEY_TAB) {
		eve->ExecuteDefaultEventAction(true);
		return;
	}
	manager->TabPressedOnControl(this);
}

template <class TWidget> \
XTabStopWidget<TWidget>::~XTabStopWidget() { }