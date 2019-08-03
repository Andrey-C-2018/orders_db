#include "XDateField.h"

XDateField::XDateField(ITabStopManager *manager) noexcept : \
							XTabStopWidget<XEdit>(manager), \
							date_manager(this) { }

XDateField::XDateField(ITabStopManager *manager, XWindow *parent, \
						const int flags, const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) : \
							XTabStopWidget<XEdit>(manager), \
							date_manager(this) {

	CreateInternal(parent, flags, label, x, y, width, height);
}

void XDateField::Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) {

	CreateInternal(parent, flags, label, x, y, width, height);
}

void XDateField::OnChange(XCommandEvent *eve) {

	date_manager.OnChange(eve);
}

XDateField::~XDateField() { }
