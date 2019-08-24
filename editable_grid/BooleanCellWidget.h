#pragma once
#include "EditableCellWidget.h"

class CBooleanCellWidget : public CEditableCellWidget {
	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

public:
	CBooleanCellWidget();

	CBooleanCellWidget(const CBooleanCellWidget &obj) = delete;
	CBooleanCellWidget(CBooleanCellWidget &&obj) = default;
	CBooleanCellWidget &operator=(CBooleanCellWidget &&obj) = default;
	CBooleanCellWidget &operator=(const CBooleanCellWidget &obj) = delete;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CBooleanCellWidget();
};

