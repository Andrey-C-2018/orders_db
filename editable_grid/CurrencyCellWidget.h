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
	CCurrencyCellWidget();

	CCurrencyCellWidget(const CCurrencyCellWidget &obj) = delete;
	CCurrencyCellWidget(CCurrencyCellWidget &&obj) = default;
	CCurrencyCellWidget &operator=(CCurrencyCellWidget &&obj) = default;
	CCurrencyCellWidget &operator=(const CCurrencyCellWidget &obj) = delete;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;

	virtual ~CCurrencyCellWidget();
};

