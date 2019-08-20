#pragma once
#include <db_ext/IInsertBinder.h>

class XWidget;

class CIntInsertBinder : public IInsertBinder {
	int int_value;
public:
	CIntInsertBinder(const int value) : int_value(value) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	virtual ~CIntInsertBinder() { }
};

class CVisualInsertBinder : public IInsertBinder {
	bool deallocate_widget_object;
protected:
	XWidget *widget;
public:
	CVisualInsertBinder(XWidget *widget_, const bool deallocate_widget_object_);
	virtual ~CVisualInsertBinder();
};

class UIIntInsertBinder : public CVisualInsertBinder {
public:
	UIIntInsertBinder(XWidget *widget_, const bool deallocate_widget_object_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	virtual ~UIIntInsertBinder();
};

class UITextInsertBinder : public CVisualInsertBinder {
public:
	UITextInsertBinder(XWidget *widget_, const bool deallocate_widget_object_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	virtual ~UITextInsertBinder();
};

class UIDateInsertBinder : public CVisualInsertBinder {
public:
	UIDateInsertBinder(XWidget *widget_, const bool deallocate_widget_object_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	virtual ~UIDateInsertBinder();
};

class CDbComboBox;

class CDbComboBoxInsertBinder : public CVisualInsertBinder {
	CDbComboBox *db_combo;
public:
	CDbComboBoxInsertBinder(CDbComboBox *db_combo_, const bool deallocate_widget_object_);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	virtual ~CDbComboBoxInsertBinder();
};