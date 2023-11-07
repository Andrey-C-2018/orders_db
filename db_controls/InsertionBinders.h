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
	const bool nullable;
public:
	UITextInsertBinder(XWidget *widget_, const bool deallocate_widget_object_, \
						bool nullable = false);

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

class XComboBox;

class CComboBoxInsertBinder : public UITextInsertBinder {
	XComboBox *combo;
	bool allow_empty;
	Tstring value_if_empty;
public:
	CComboBoxInsertBinder(XComboBox *combo_, const bool deallocate_widget_object_, \
							const bool allow_empty);

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;
	inline void setValueIfEmpty(const Tchar *value_if_empty) { this->value_if_empty = value_if_empty; }

	virtual ~CComboBoxInsertBinder();
};

class CDbComboBox;

class CDbComboBoxInsertBinder : public CVisualInsertBinder {
	CDbComboBox *db_combo;
	bool allow_empty;
	int value_if_empty;
public:
	CDbComboBoxInsertBinder(CDbComboBox *db_combo_, const bool deallocate_widget_object_, \
							const bool allow_empty);

	inline void setValueIfEmpty(const int value_if_empty) {	this->value_if_empty = value_if_empty; }
	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override;

	virtual ~CDbComboBoxInsertBinder();
};