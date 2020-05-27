#pragma once
#include <db_ext/IBinder.h>

class XWidget;
class CDbComboBox;

class CIntWidgetBinderControl : public IBinder {
	XWidget *widget;
public:
	CIntWidgetBinderControl(XWidget *widget_);

	void bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CIntWidgetBinderControl();
};

class CDbComboBoxBinderControl : public IBinder {
	CDbComboBox *combobox;
public:
	CDbComboBoxBinderControl(CDbComboBox *combobox_);

	void bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CDbComboBoxBinderControl();
};