#pragma once
#include <xwindows_ex/XCtrlInputFilter.h>
#include "EditableCellWidget.h"

class CDateCellWidget : public CEditableCellWidget {
	XCtrlInputFilter date_filter;
	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

	void OnChange(XCommandEvent *eve);

public:
	CDateCellWidget();

	CDateCellWidget(const CDateCellWidget &obj) = delete;
	CDateCellWidget(CDateCellWidget &&obj) = default;
	CDateCellWidget &operator=(CDateCellWidget &&obj) = default;
	CDateCellWidget &operator=(const CDateCellWidget &obj) = delete;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CDateCellWidget();
};

