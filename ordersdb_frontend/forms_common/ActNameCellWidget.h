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
	inline bool InternalValidate(ImmutableString<Tchar> act_name) const;
public:
	CActNameCellWidget();

	CActNameCellWidget(const CActNameCellWidget &obj) = delete;
	CActNameCellWidget(CActNameCellWidget &&obj) = default;
	CActNameCellWidget &operator=(const CActNameCellWidget &obj) = delete;
	CActNameCellWidget &operator=(CActNameCellWidget &&obj) = default;
	
	void SetOnChangeHandler(XEventHandlerData on_change) override;

	bool Validate() const;
	bool Validate(ImmutableString<Tchar> label) const;

	virtual ~CActNameCellWidget();
};

//*****************************************************

bool CActNameCellWidget::InternalValidate(ImmutableString<Tchar> act_name) const {

	Tstring error_str;
	bool cancel = !validator.validate(act_name, error_str);
	if (cancel || !error_str.empty()) {
		if (!error_str.empty()) ErrorBox(error_str.c_str());
		return false;
	}

	validator.refreshActNameWidget(ignore_change_event);
	return true;
}