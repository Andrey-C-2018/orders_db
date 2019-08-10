#include <basic/XConv.h>
#include "IntegerCellWidget.h"

enum {
	MAX_INT_FIELD_LEN = getDigitsCountOfType<int>()
};

CIntegerCellWidget::CIntegerCellWidget(const size_t max_length_) : max_length(max_length_) {

	assert(max_length <= MAX_INT_FIELD_LEN);
}

void CIntegerCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
								FL_EDIT_ONLY_NUMBERS | FL_WINDOW_BORDERED, \
					label, x, y, width, height);
	XEdit::SetEditMaxLength(max_length);
}

bool CIntegerCellWidget::Validate() const {

	std::vector<Tchar> v;
	auto label = GetLabel(v);

	int error = 0;
	XConv::ToInt(label.str, error);

	if (error) {
		Tstring err_str = _T("Введене значення не є числом: '");
		err_str += label.str;
		err_str += _T("'");
		ErrorBox(err_str.c_str());
	}

	return !error;
}

bool CIntegerCellWidget::Validate(ImmutableString<Tchar> label) const {

	int error = 0;
	XConv::ToInt(label.str, error);

	if (error) {
		Tstring err_str = _T("Введене значення не є числом: '");
		err_str += label.str;
		err_str += _T("'");
		ErrorBox(err_str.c_str());
	}

	return !error;
}

CIntegerCellWidget::~CIntegerCellWidget() { }
