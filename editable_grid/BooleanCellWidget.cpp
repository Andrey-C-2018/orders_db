#include "BooleanCellWidget.h"

CBooleanCellWidget::CBooleanCellWidget() { }

void CBooleanCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
					FL_EDIT_ONLY_NUMBERS | FL_WINDOW_BORDERED, \
					label, x, y, width, height);
	XEdit::SetEditMaxLength(1);
}

bool CBooleanCellWidget::Validate() const {

	std::vector<Tchar> v;
	auto label = GetLabel(v);

	if (!label.size || (label.size && label.str[0] != _T('0') && label.str[0] != _T('1'))) {
		Tstring err_str = _T("Недопустиме значення логічного поля: '");
		err_str += label.str;
		err_str += _T("'. Допустимі значення - 0 або 1");
		ErrorBox(err_str.c_str());
		return false;
	}

	return true;
}

bool CBooleanCellWidget::Validate(ImmutableString<Tchar> label) const {

	if (!label.size || (label.size && label.str[0] != _T('0') && label.str[0] != _T('1'))) {
		Tstring err_str = _T("Недопустиме значення логічного поля: '");
		err_str += label.str;
		err_str += _T("'. Допустимі значення - 0 або 1");
		ErrorBox(err_str.c_str());
		return false;
	}

	return true;
}

CBooleanCellWidget::~CBooleanCellWidget() { }
