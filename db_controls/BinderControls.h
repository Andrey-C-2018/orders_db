#pragma once
#include <basic/tstring.h>
#include <db_ext/IBinder.h>

class XWidget;
class CDbComboBox;

class CErrCheckingBinder : public IBinder {
protected:
	Tstring err_str;
public:
	CErrCheckingBinder() { }
	virtual ~CErrCheckingBinder() { }
};

class CIntWidgetBinderControl : public CErrCheckingBinder {
	XWidget *widget;
public:
	CIntWidgetBinderControl(XWidget *widget_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CIntWidgetBinderControl();
};

class CStringWidgetBinderControl : public IBinder {
	XWidget *widget;
public:
	CStringWidgetBinderControl(XWidget *widget_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CStringWidgetBinderControl();
};

class CDateWidgetBinderControl : public CErrCheckingBinder {
	XWidget *widget;
public:
	CDateWidgetBinderControl(XWidget *widget_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CDateWidgetBinderControl();
};

class CDbComboBoxBinderControl : public CErrCheckingBinder {
	CDbComboBox *combobox;
public:
	CDbComboBoxBinderControl(CDbComboBox *combobox_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CDbComboBoxBinderControl();
};