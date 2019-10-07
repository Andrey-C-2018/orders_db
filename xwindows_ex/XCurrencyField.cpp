#include "XCurrencyField.h"

XCurrencyField::XCurrencyField(ITabStopManager *manager) noexcept : \
									XTabStopEdit(manager), \
									filter(this, _T(','), _T('.'), true, nullptr) { }

XCurrencyField::XCurrencyField(ITabStopManager *manager, XWindow *parent, \
								const int flags, const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) : \
									XTabStopEdit(manager) , \
									filter(this, _T(','), _T('.'), true, nullptr) {

	CreateInternal(parent, flags, label, x, y, width, height);
}

void XCurrencyField::Create(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) {

	CreateInternal(parent, flags, label, x, y, width, height);
}

void XCurrencyField::OnChange(XCommandEvent *eve) {

	filter.OnChange(eve);
}

XCurrencyField::~XCurrencyField() { }
