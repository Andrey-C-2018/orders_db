#pragma once

class XWidget;

struct ITabStopAction {
	virtual void exec(XWidget *widget) = 0;
	virtual ~ITabStopAction() { }
};