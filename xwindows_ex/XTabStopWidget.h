#pragma once
#include <xwindows/XWidget.h>
#include "ITabStopManager.h"
#include "ITabStopAction.h"

template <class TWidget> \
class XTabStopWidget : public TWidget {
	ITabStopManager *manager;
	ITabStopAction *action;

protected:
	inline void setTabStopAction(ITabStopAction *action);

	inline void OnKeyPressInternal(XKeyboardEvent *eve);
	virtual void OnKeyPress(XKeyboardEvent *eve);

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
									manager(manager_), action(nullptr) { }

template <class TWidget> \
XTabStopWidget<TWidget>::XTabStopWidget(ITabStopManager *manager_, XWindow *parent, \
										const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) : \
									XEdit(parent, flags, label, x, y, width, height), \
									manager(manager_), action(nullptr) {

	assert(manager);
	manager->RegisterTabStopControl(this);

	OverrideWindowEvent(EVT_KEYDOWN, XEventHandlerData(this, &XTabStopWidget::OnKeyPress));
}

template <class TWidget> \
void XTabStopWidget<TWidget>::setTabStopAction(ITabStopAction *action) {

	this->action = action;
}

template <class TWidget> \
void XTabStopWidget<TWidget>::Create(XWindow *parent, const int flags, \
									const Tchar *label, \
									const int x, const int y, \
									const int width, const int height) {

	assert(manager);
	manager->RegisterTabStopControl(this, action);

	TWidget::Create(parent, flags, label, x, y, width, height);
	OverrideWindowEvent(EVT_KEYDOWN, XEventHandlerData(this, &XTabStopWidget::OnKeyPress));
}

template <class TWidget> \
void XTabStopWidget<TWidget>::OnKeyPressInternal(XKeyboardEvent *eve) {

	if (eve->GetKey() != X_VKEY_TAB) {
		eve->ExecuteDefaultEventAction(true);
		return;
	}
	manager->TabPressedOnControl(this);
}

template <class TWidget> \
void XTabStopWidget<TWidget>::OnKeyPress(XKeyboardEvent *eve) {

	OnKeyPressInternal(eve);
}

template <class TWidget> \
XTabStopWidget<TWidget>::~XTabStopWidget() { }