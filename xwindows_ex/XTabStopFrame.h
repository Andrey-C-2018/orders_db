#pragma once
#include <xwindows/XFrame.h>
#include "ITabStopManager.h"
#include "XTabStopManager.h"

class XTabStopFrame : public XFrame, public ITabStopManager {
	XTabStopManager manager;
public:
	inline XTabStopFrame() noexcept : manager(this) { }

	inline XTabStopFrame(const Tchar *class_name, const int flags, \
					const Tchar *label, const int x, const int y, \
					const int width, const int height) : \
		XFrame(class_name, flags, label, x, y, width, height), manager(this) { }

	XTabStopFrame(XTabStopFrame &&obj) = default;
	XTabStopFrame(const XTabStopFrame &obj) = delete;
	XTabStopFrame &operator=(const XTabStopFrame &obj) = delete;
	XTabStopFrame &operator=(XTabStopFrame &&obj) = default;

	void RegisterTabStopControl(XWidget *widget) override { 
		manager.RegisterTabStopControl(widget);
	}
	void RegisterTabStopControl(XWidget *widget, ITabStopAction *action) override {
		manager.RegisterTabStopControl(widget, action);
	}
	void TabPressedOnControl(XWidget *widget) override {
		manager.TabPressedOnControl(widget);
	}

	virtual ~XTabStopFrame() { }
};
