#pragma once

class XWidget;
class XKeyboardEvent;
struct ITabStopAction;

struct ITabStopManager {
	virtual void RegisterTabStopControl(XWidget *widget) = 0;
	virtual void RegisterTabStopControl(XWidget *widget, ITabStopAction *action) = 0;
	virtual void TabPressedOnControl(XWidget *widget) = 0;
};
