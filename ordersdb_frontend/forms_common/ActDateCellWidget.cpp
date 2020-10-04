#include "ActDateCellWidget.h"

CActDateCellWidget::CActDateCellWidget(std::shared_ptr<CDbTable> db_table) : \
										CDateCellWidget(true), \
										validator(db_table, nullptr) { }

bool CActDateCellWidget::Validate() const {

	bool date_validated = CDateCellWidget::Validate();
	return date_validated && ValidateActDate(getCachedLabel());
}

bool CActDateCellWidget::Validate(ImmutableString<Tchar> label) const {

	bool date_validated = CDateCellWidget::Validate(label);
	return date_validated && ValidateActDate(label);
}

CActDateCellWidget::~CActDateCellWidget() { }