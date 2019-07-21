#pragma once
#include <vector>

class XWindow;
class XWidget;

class XTabStopManager {
	XWindow *tabstop_parent;
	std::vector<XWidget*> tabstop_widgets;

public:
	XTabStopManager(XWindow *tabstop_parent_);

	XTabStopManager(const XTabStopManager &obj) = default;
	XTabStopManager(XTabStopManager &&obj) = default;
	XTabStopManager &operator=(const XTabStopManager &obj) = default;
	XTabStopManager &operator=(XTabStopManager &&obj) = default;

	void RegisterTabStopControl(XWidget *widget);
	void TabPressedOnControl(XWidget *widget);

	virtual ~XTabStopManager();
};

