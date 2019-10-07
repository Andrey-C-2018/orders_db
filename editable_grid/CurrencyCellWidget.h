#pragma once
#include <xwindows_ex/XCtrlInputFilter.h>
#include "EditableCellWidget.h"

class CCurrencyCellWidget : public CEditableCellWidget {
	XCtrlInputFilter currency_filter;
	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

	void OnChange(XCommandEvent *eve);

public:
	CCurrencyCellWidget(const bool null_allowed);

	CCurrencyCellWidget(const CCurrencyCellWidget &obj) = delete;
	CCurrencyCellWidget(CCurrencyCellWidget &&obj) = default;
	CCurrencyCellWidget &operator=(CCurrencyCellWidget &&obj) = default;
	CCurrencyCellWidget &operator=(const CCurrencyCellWidget &obj) = delete;

	void SetOnChangeHandler(XEventHandlerData on_change) override;

	virtual ~CCurrencyCellWidget();
};

