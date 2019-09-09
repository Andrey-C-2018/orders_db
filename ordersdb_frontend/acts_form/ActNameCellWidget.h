#pragma once
#include <editable_grid/EditableCellWidget.h>
#include "Validators.h"

class CActNameCellWidget : public CEditableCellWidget {
	mutable CActNameValidator validator;
	mutable bool ignore_change_event;
	mutable std::vector<Tchar> label_cache;

	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

	void OnChange(XCommandEvent *eve);
public:
	CActNameCellWidget();

	CActNameCellWidget(const CActNameCellWidget &obj) = delete;
	CActNameCellWidget(CActNameCellWidget &&obj) = default;
	CActNameCellWidget &operator=(const CActNameCellWidget &obj) = delete;
	CActNameCellWidget &operator=(CActNameCellWidget &&obj) = default;
	
	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;

	bool Validate() const;
	bool Validate(ImmutableString<Tchar> label) const;

	virtual ~CActNameCellWidget();
};

