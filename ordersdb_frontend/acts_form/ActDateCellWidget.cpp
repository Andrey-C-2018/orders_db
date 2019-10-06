#include "ActDateCellWidget.h"

CActDateCellWidget::CActDateCellWidget(std::shared_ptr<CDbTable> db_table) : \
										validator(db_table, nullptr) { }

bool CActDateCellWidget::Validate() const {

	auto label = XEdit::GetLabel(label_cache);
	ImmutableString<Tchar> date_str(label, label_cache.size());

	return InternalValidate(date_str);
}

bool CActDateCellWidget::Validate(ImmutableString<Tchar> label) const {

	return InternalValidate(label);
}

CActDateCellWidget::~CActDateCellWidget() { }