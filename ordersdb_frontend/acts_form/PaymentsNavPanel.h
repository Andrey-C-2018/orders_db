#pragma once
#include <xwindows/XPanel.h>
#include <xwindows/XButton.h>

class CDbNavigator;

class CPaymentsNavPanel : public XPanel {
	CDbNavigator *db_navigator;
	XButton *button_add, *button_remove;

public:
	CPaymentsNavPanel();
	virtual ~CPaymentsNavPanel();
};

