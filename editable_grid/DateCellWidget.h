#pragma once
#include <date/Date.h>
#include <xwindows_ex/XCtrlInputFilter.h>
#include "EditableCellWidget.h"

class CDateCellWidget : public CEditableCellWidget {
	XCtrlInputFilter date_filter;
	CDelegate on_change_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;
	bool null_value_allowed;

	void OnChange(XCommandEvent *eve);
	inline bool InternalValidate(ImmutableString<Tchar> validated_label) const;

public:
	CDateCellWidget();
	CDateCellWidget(const bool null_value_allowed_);

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

//*****************************************************

bool CDateCellWidget::InternalValidate(ImmutableString<Tchar> validated_label) const {

	CDate dt(validated_label.str, CDate::GERMAN_FORMAT);

	bool validated = true;
	if (dt.isNull()) {

		if (!validated_label.size) {
			if (!null_value_allowed) {
				ErrorBox(_T("Порожні значення дати заборонені"));
				return false;
			}
			return true;
		}

		Tstring err_str = _T("Невірний формат дати: '");
		err_str += validated_label.str;
		err_str += _T("'");
		ErrorBox(err_str.c_str());
		validated = false;
	}

	return validated;
}