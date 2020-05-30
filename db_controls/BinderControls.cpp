#include "BinderControls.h"
#include <basic/XConv.h>
#include <basic/Exception.h>
#include <db/IDbBindingTarget.h>
#include <xwindows/XWidget.h>
#include "DbComboBox.h"
#include <assert.h>

CIntWidgetBinderControl::CIntWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

bool CIntWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) {

	int err = 0;
	auto text = widget->GetLabel();
	int i = XConv::ToInt(text, err);

	if (err) {
		Tchar buffer[4];

		err_str = _T("Не є цілим числом: ");
		err_str += text;
		err_str += _T(" , № параметру: ");
		err_str += XConv::ToString(param_no, buffer);
		WarningBox(err_str.c_str());
	}
	else
		binding_target->bindValue(param_no, i);

	return !err;
}

CIntWidgetBinderControl::~CIntWidgetBinderControl() { }

//*****************************************************

CStringWidgetBinderControl::CStringWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

bool CStringWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no) {

	binding_target->bindValue(param_no, widget->GetLabel());
	return true;
}

CStringWidgetBinderControl::~CStringWidgetBinderControl() { }

//*****************************************************

CDateWidgetBinderControl::CDateWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

bool CDateWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) {

	auto date_str = widget->GetLabel();
	CDate date;
	
	bool ok = date.setDateGerman(date_str);

	if (!ok) {
		Tchar buffer[4];

		err_str = _T("Невірний формат дати: ");
		err_str += date_str;
		err_str += _T(" , № параметру: ");
		err_str += XConv::ToString(param_no, buffer);
		WarningBox(err_str.c_str());
	}
	else
		binding_target->bindValue(param_no, date);

	return ok;
}

CDateWidgetBinderControl::~CDateWidgetBinderControl() { }

//*****************************************************

CDbComboBoxBinderControl::CDbComboBoxBinderControl(CDbComboBox *combobox_) : \
													combobox(combobox_) {

	assert(combobox);
}

bool CDbComboBoxBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) {

	int i = combobox->getPrimaryKeyAsInteger();
	binding_target->bindValue(param_no, i);
	return true;
}

CDbComboBoxBinderControl::~CDbComboBoxBinderControl() { }
