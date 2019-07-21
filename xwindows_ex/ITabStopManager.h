#pragma once

class XWidget;
class XKeyboardEvent;

struct ITabStopManager {
	virtual void RegisterTabStopControl(XWidget *widget) = 0;
	virtual void TabPressedOnControl(XWidget *widget) = 0;
};
