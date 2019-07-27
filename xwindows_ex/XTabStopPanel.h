#pragma once
#include <xwindows/XPanel.h>
#include "ITabStopManager.h"
#include "XTabStopManager.h"

class XTabStopPanel : public XPanel, public ITabStopManager {
	XTabStopManager manager;
public:
	inline XTabStopPanel() noexcept : manager(this) { }

	inline XTabStopPanel(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) : \
		XPanel(parent, flags, label, x, y, width, height), manager(this) { }

	XTabStopPanel(XTabStopPanel &&obj) = default;
	XTabStopPanel(const XTabStopPanel &obj) = delete;
	XTabStopPanel &operator=(const XTabStopPanel &obj) = delete;
	XTabStopPanel &operator=(XTabStopPanel &&obj) = default;

	void RegisterTabStopControl(XWidget *widget) override {
		manager.RegisterTabStopControl(widget);
	}
	void TabPressedOnControl(XWidget *widget) override {
		manager.TabPressedOnControl(widget);
	}

	virtual ~XTabStopPanel() { }
};

