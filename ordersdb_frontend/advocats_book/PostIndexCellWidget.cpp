#include "PostIndexCellWidget.h"
#include "Validators.h"

CPostIndexCellWidget::CPostIndexCellWidget() { }

bool CPostIndexCellWidget::Validate() const {

	XWindow::GetLabel(label_cache);
	ImmutableString<Tchar> validated_label(&label_cache[0], label_cache.size());
	err_str.clear();

	CPostIndexValidator::validate(validated_label, err_str);
	if (!err_str.empty()) {
		ErrorBox(err_str.c_str());
		return false;
	}
	return true;
}

bool CPostIndexCellWidget::Validate(ImmutableString<Tchar> label) const {

	CPostIndexValidator::validate(label, err_str);
	if (!err_str.empty()) {
		ErrorBox(err_str.c_str());
		return false;
	}
	return true;
}

CPostIndexCellWidget::~CPostIndexCellWidget() { }
