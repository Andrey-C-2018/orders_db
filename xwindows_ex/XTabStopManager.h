#pragma once
#include <vector>
#include "ITabStopAction.h"

class XWindow;
class XWidget;

class XTabStopManager {
	enum {
		DEF_TABSTOP_WIDGETS_COUNT = 10
	};

	XWindow *tabstop_parent;
	struct CTabStopItem {
		XWidget *widget;
		ITabStopAction *tabstop_action;

		inline bool operator==(const CTabStopItem &obj) const {

			return widget == obj.widget;
		}
	};
	std::vector<CTabStopItem> tabstop_widgets;

public:
	XTabStopManager(XWindow *tabstop_parent_);

	XTabStopManager(const XTabStopManager &obj) = default;
	XTabStopManager(XTabStopManager &&obj) = default;
	XTabStopManager &operator=(const XTabStopManager &obj) = default;
	XTabStopManager &operator=(XTabStopManager &&obj) = default;

	void RegisterTabStopControl(XWidget *widget);
	void RegisterTabStopControl(XWidget *widget, ITabStopAction *tabstop_action);
	void TabPressedOnControl(XWidget *widget);

	virtual ~XTabStopManager();
};

