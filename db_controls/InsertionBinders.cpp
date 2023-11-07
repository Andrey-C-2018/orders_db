#include <basic/TextConv.h>
#include <basic/XConv.h>
#include <db/IDbBindingTarget.h>
#include <xwindows/XWidget.h>
#include "DbComboBox.h"
#include "InsertionBinders.h"

bool CIntInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
							Params &params, const Tchar *field_name) {

	binding_target->bindValue(params.param_no, int_value);
	++params.param_no;
	return true;
}

//*****************************************************

CVisualInsertBinder::CVisualInsertBinder(XWidget *widget_, \
										const bool deallocate_widget_object_) : \
						widget(widget_), deallocate_widget_object(deallocate_widget_object_) {

	assert(widget);
}

CVisualInsertBinder::~CVisualInsertBinder() {

	if (deallocate_widget_object && widget && !widget->IsCreated()) delete widget;
}

//*****************************************************

UIIntInsertBinder::UIIntInsertBinder(XWidget *widget_, const bool deallocate_widget_object_) : \
										CVisualInsertBinder(widget_, deallocate_widget_object_) { }

bool UIIntInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
								Params &params, const Tchar *field_name) {

	size_t size;
	auto text = widget->GetLabel(size);

	int error;
	int value = XConv::ToInt(text, error);
	if (error) {
		params.error_str += field_name;
		params.error_str += _T(": ������� �������� �� � ������:");
		params.error_str += text;
		params.error_str += _T('\n');
		++params.param_no;
		return true;
	}

	binding_target->bindValue(params.param_no, value);
	++params.param_no;
	return true;
}

UIIntInsertBinder::~UIIntInsertBinder() { }

//*****************************************************

UITextInsertBinder::UITextInsertBinder(XWidget *widget_, \
										const bool deallocate_widget_object_, \
										bool nullable_) : \
								CVisualInsertBinder(widget_, deallocate_widget_object_), \
								nullable(nullable_) { }

bool UITextInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
								Params &params, const Tchar *field_name) {

	size_t size;
	auto text = widget->GetLabel(size);
	if (text && text[0] == _T('\0')) {

		if (nullable)
			binding_target->bindNull(params.param_no);
		else {
			params.error_str += field_name;
			params.error_str += _T(" �� ���� ���� �������\n");
		}
		++params.param_no;
		return true;
	}

	std::string text_utf8;
	UCS16_ToUTF8(text, (int)size, text_utf8);
	binding_target->bindValue(params.param_no, text_utf8.c_str());

	++params.param_no;
	return true;
}

UITextInsertBinder::~UITextInsertBinder() { }

//*****************************************************

UIDateInsertBinder::UIDateInsertBinder(XWidget *widget_, \
										const bool deallocate_widget_object_) : \
							CVisualInsertBinder(widget_, deallocate_widget_object_) { }

bool UIDateInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
								Params &params, const Tchar *field_name) {

	size_t size;
	auto date_str = widget->GetLabel(size);
	if (size == 0) {
		binding_target->bindNull(params.param_no);
		++params.param_no;
		return true;
	}

	CDate date(date_str, CDate::GERMAN_FORMAT);
	if (date.isNull()) {

		params.error_str += field_name;
		params.error_str += _T(": ������� ������ ����\n");
		++params.param_no;
		return true;
	}
	if (date > CDate::now()) {

		params.error_str += field_name;
		params.error_str += _T(": '");

		Tchar buffer[CDate::GERMAN_FORMAT_LEN + 1];
		date.toStringGerman(buffer);
		params.error_str += buffer;

		params.error_str += _T("' ���� � �����������\n");
		++params.param_no;
		return true;
	}

	binding_target->bindValue(params.param_no, date);
	++params.param_no;
	return true;
}

UIDateInsertBinder::~UIDateInsertBinder() { }

//*****************************************************

CComboBoxInsertBinder::CComboBoxInsertBinder(XComboBox *combo_, \
											const bool deallocate_widget_object_, \
											const bool allow_empty_) : \
							UITextInsertBinder(combo_, deallocate_widget_object_), \
							combo(combo_), allow_empty(allow_empty_) { }

bool CComboBoxInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
								Params &params, const Tchar *field_name) {

	size_t sel_index = combo->GetCurrentSelectionIndex();
	if (!sel_index || sel_index == -1) {
		if (allow_empty) {
			binding_target->bindValue(params.param_no, value_if_empty.c_str());
			++params.param_no;
			return true;
		}
		params.error_str += field_name;
		params.error_str += _T(": �������� �� ������ �� �������\n");
		++params.param_no;
		return true;
	}

	return UITextInsertBinder::bind(binding_target, params, field_name);
}

CComboBoxInsertBinder::~CComboBoxInsertBinder() { }

//*****************************************************

CDbComboBoxInsertBinder::CDbComboBoxInsertBinder(CDbComboBox *db_combo_, \
												const bool deallocate_widget_object_, \
												const bool allow_empty_) : \
							CVisualInsertBinder(db_combo_, deallocate_widget_object_), \
							db_combo(db_combo_), allow_empty(allow_empty_), \
							value_if_empty(0) { }

bool CDbComboBoxInsertBinder::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									Params &params, const Tchar *field_name) {
	
	if (db_combo->isEmptySelection()) {
		if (allow_empty) {
			binding_target->bindValue(params.param_no, value_if_empty);
			++params.param_no;
			return true;
		}
		params.error_str += field_name;
		params.error_str += _T(": �������� �� ������ �� �������\n");
		++params.param_no;
		return true;
	}

	binding_target->bindValue(params.param_no, db_combo->getPrimaryKeyAsInteger());
	++params.param_no;
	return true;
}

CDbComboBoxInsertBinder::~CDbComboBoxInsertBinder() { }