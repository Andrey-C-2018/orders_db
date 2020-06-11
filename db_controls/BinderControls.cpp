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

size_t CIntWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
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

	return (size_t)(!err);
}

void CIntWidgetBinderControl::reset() {

	widget->SetLabel(_T(""));
}

CIntWidgetBinderControl::~CIntWidgetBinderControl() { }

//*****************************************************

CStringWidgetBinderControl::CStringWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

size_t CStringWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no) {

	binding_target->bindValue(param_no, widget->GetLabel());
	return 1;
}

void CStringWidgetBinderControl::reset() {

	widget->SetLabel(_T(""));
}

CStringWidgetBinderControl::~CStringWidgetBinderControl() { }

//*****************************************************

CDateWidgetBinderControl::CDateWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

size_t CDateWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
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

	return (size_t)ok;
}

void CDateWidgetBinderControl::reset() {

	widget->SetLabel(_T(""));
}

CDateWidgetBinderControl::~CDateWidgetBinderControl() { }

//*****************************************************

CDbComboBoxBinderControl::CDbComboBoxBinderControl(CDbComboBox *combobox_) : \
													combobox(combobox_) {

	assert(combobox);
}

size_t CDbComboBoxBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) {

	int i = combobox->getPrimaryKeyAsInteger();
	binding_target->bindValue(param_no, i);
	return 1;
}

void CDbComboBoxBinderControl::reset() {

	combobox->selectEmptyValue();
}

CDbComboBoxBinderControl::~CDbComboBoxBinderControl() { }
