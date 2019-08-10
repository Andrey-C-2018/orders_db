#pragma once
#include "EditableCellWidget.h"

class CIntegerCellWidget : public CEditableCellWidget {
	const size_t max_length;
	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

public:
	CIntegerCellWidget(const size_t max_length_);

	CIntegerCellWidget(const CIntegerCellWidget &obj) = delete;
	CIntegerCellWidget(CIntegerCellWidget &&obj) = default;
	CIntegerCellWidget &operator=(CIntegerCellWidget &&obj) = default;
	CIntegerCellWidget &operator=(const CIntegerCellWidget &obj) = delete;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CIntegerCellWidget();
};
