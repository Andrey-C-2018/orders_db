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

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;
	size_t affectedParamsCount() const override { return 1; }
	void reset() override;

	virtual ~CIntWidgetBinderControl();
};

class CStringWidgetBinderControl : public IBinder {
	XWidget *widget;
public:
	CStringWidgetBinderControl(XWidget *widget_);

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;
	size_t affectedParamsCount() const override { return 1; }
	void reset() override;

	virtual ~CStringWidgetBinderControl();
};

class CDateWidgetBinderControl : public CErrCheckingBinder {
	XWidget *widget;
public:
	CDateWidgetBinderControl(XWidget *widget_);

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;
	size_t affectedParamsCount() const override { return 1; }
	void reset() override;

	virtual ~CDateWidgetBinderControl();
};

class CDbComboBoxBinderControl : public CErrCheckingBinder {
	CDbComboBox *combobox;
public:
	CDbComboBoxBinderControl(CDbComboBox *combobox_);

	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;
	size_t affectedParamsCount() const override { return 1; }
	void reset() override;

	virtual ~CDbComboBoxBinderControl();
};