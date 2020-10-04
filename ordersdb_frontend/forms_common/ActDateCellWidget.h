#pragma once
#include <editable_grid/DateCellWidget.h>
#include "Validators.h"

class CActDateCellWidget : public CDateCellWidget {
	CActDateValidator validator;
	
	inline bool ValidateActDate(ImmutableString<Tchar> date_str) const;

public:
	CActDateCellWidget(std::shared_ptr<CDbTable> db_table);

	CActDateCellWidget(const CActDateCellWidget &obj) = delete;
	CActDateCellWidget(CActDateCellWidget &&obj) = default;
	CActDateCellWidget &operator=(const CActDateCellWidget &obj) = delete;
	CActDateCellWidget &operator=(CActDateCellWidget &&obj) = default;

	bool Validate() const;
	bool Validate(ImmutableString<Tchar> label) const;

	virtual ~CActDateCellWidget();
};

//*****************************************************

bool CActDateCellWidget::ValidateActDate(ImmutableString<Tchar> date_str) const {

	CDate dt(date_str.str, CDate::GERMAN_FORMAT);
	if (dt.isNull()) return true;

	Tstring error_str;
	bool cancel = !validator.validate(date_str, dt, error_str, _T("Дата акта"));
	if (!error_str.empty()) ErrorBox(error_str.c_str());

	return !cancel && error_str.empty();
}