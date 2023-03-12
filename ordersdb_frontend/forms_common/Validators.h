#pragma once
#include <basic/XConv.h>
#include <date/Date.h>
#include <db_ext/DbTable.h>
#include <xwindows/XWidget.h>

class CActNameValidator final {
	enum { MAX_ACT_NAME_LEN = 25 };

	Tstring act_name;
	bool is_act_name_corrected;
	XWidget *act_name_holder;
public:
	CActNameValidator() : is_act_name_corrected(false), \
						act_name_holder(nullptr) { }

	CActNameValidator(XWidget *act_name_holder_) : \
						is_act_name_corrected(false), \
						act_name_holder(act_name_holder_) { }

	CActNameValidator(const CActNameValidator &obj) = default;
	CActNameValidator(CActNameValidator &&obj) = default;
	CActNameValidator &operator=(const CActNameValidator &obj) = default;
	CActNameValidator &operator=(CActNameValidator &&obj) = default;

	inline bool validate(ImmutableString<Tchar> act_str, \
								Tstring &error_str) {

		if (act_str.size > MAX_ACT_NAME_LEN) {
			error_str += _T("��'� ���� �� ���� ���� ������ �� ");
			Tchar buffer[3];
			error_str += XConv::ToString((size_t)MAX_ACT_NAME_LEN, buffer);
			error_str += _T(" ����.\n");
			return true;
		}
		act_name.clear();
		act_name.reserve(act_str.size);
		is_act_name_corrected = false;

		for (size_t i = 0; i < act_str.size; ++i) {
			Tchar ch = ToUpper(act_str.str[i]);
			is_act_name_corrected = is_act_name_corrected || \
									ch != act_str.str[i] || ch == _T(' ');

			if (ch == _T(' ')) continue;

			if (!((ch >= _T('0') && ch <= _T('9')) || \
				(ch >= _T('�') && ch <= _T('�')) || \
				ch == _T('.') || ch == _T('�'))) {

				error_str += _T("������ ��'� ����: ");
				error_str += act_str.str;
				error_str += _T('\n');
				return true;
			}
			act_name += ch;
		}

		return true;
	}

	inline void refreshActNameWidget(bool &ignore_change_event) {
		
		assert(act_name_holder);
		if (is_act_name_corrected) {
			ignore_change_event = true;
			act_name_holder->SetLabel(act_name.c_str());
			ignore_change_event = false;
		}
	}

	inline const Tchar *getActName() const { return act_name.c_str(); }
};

//*****************************************************

class CActDateValidatorBasic {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

protected:
	XWidget *act_reg_date_widget;
	inline bool validate(ImmutableString<Tchar> date_str, \
						const CDate &date, \
						const CDate &order_date, \
						const CDate &act_reg_date, \
						Tstring &error_str, \
						const Tchar *error_str_prefix) const;

public:
	CActDateValidatorBasic(XWidget *act_reg_date_widget_) : \
							act_reg_date_widget(act_reg_date_widget_) { }

	CActDateValidatorBasic(const CActDateValidatorBasic &obj) = default;
	CActDateValidatorBasic(CActDateValidatorBasic &&obj) = default;
	CActDateValidatorBasic &operator=(const CActDateValidatorBasic &obj) = default;
	CActDateValidatorBasic &operator=(CActDateValidatorBasic &&obj) = default;

	virtual ~CActDateValidatorBasic() { }
};

class CActDateValidator : public CActDateValidatorBasic {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	size_t order_date_no, act_reg_dt_no;
public:
	CActDateValidator(std::shared_ptr<CDbTable> db_table_, \
						XWidget *act_reg_date_widget_) : \
			CActDateValidatorBasic(act_reg_date_widget_), \
			db_table(db_table_), \
			order_date_no(INVALID_FIELD_INDEX), \
			act_reg_dt_no(INVALID_FIELD_INDEX) {
	
		const CMetaInfo &meta_info = db_table->getMetaInfo();
		order_date_no = meta_info.getFieldIndexByName("order_date");

		if (!act_reg_date_widget_) 
			act_reg_dt_no = meta_info.getFieldIndexByName("act_reg_date");
	}

	CActDateValidator(const CActDateValidator &obj) = default;
	CActDateValidator(CActDateValidator &&obj) = default;
	CActDateValidator &operator=(const CActDateValidator &obj) = default;
	CActDateValidator &operator=(CActDateValidator &&obj) = default;

	inline bool validate(ImmutableString<Tchar> date_str, \
							const CDate &date, \
							Tstring &error_str, \
							const Tchar *error_str_prefix) const;

	virtual ~CActDateValidator() { }
};

class CActDateValidatorNoDb : public CActDateValidatorBasic {
	XWidget *order_date_widget;

public:
	CActDateValidatorNoDb(XWidget *order_date_widget_, \
							XWidget *act_reg_date_widget_) : \
					CActDateValidatorBasic(act_reg_date_widget_), \
					order_date_widget(order_date_widget_) { }

	CActDateValidatorNoDb(const CActDateValidatorNoDb &obj) = default;
	CActDateValidatorNoDb(CActDateValidatorNoDb &&obj) = default;
	CActDateValidatorNoDb &operator=(const CActDateValidatorNoDb &obj) = default;
	CActDateValidatorNoDb &operator=(CActDateValidatorNoDb &&obj) = default;

	inline bool validate(ImmutableString<Tchar> date_str, \
							const CDate &date, \
							Tstring &error_str, \
							const Tchar *error_str_prefix) const;

	virtual ~CActDateValidatorNoDb() { }
};

//*****************************************************

bool CActDateValidatorBasic::validate(ImmutableString<Tchar> date_str, \
										const CDate &date, \
										const CDate &order_date, \
										const CDate &act_reg_date, \
										Tstring &error_str, \
										const Tchar *error_str_prefix) const {

	assert(!order_date.isNull());
	if (date.isNull()) {

		error_str += error_str_prefix;
		error_str += _T(": ������� ������ ����: ");
		error_str += date_str.str;
		error_str += _T('\n');
		return true;
	}
	if (date > CDate::now()) {
		int option = _plMessageBoxYesNo(_T("���� ���� � �����������. ������������?"));
		if (option == IDNO) return false;
	}
	if (date < CDate(1, 1, 2013)) {
		error_str += error_str_prefix;
		error_str += _T(": �� ���� ���� ������ �� 01.01.2013\n");
		return true;
	}

	if (date < order_date) {
		error_str += error_str_prefix;
		error_str += _T(": �� ���� ���� ������ �� ���� ���������\n");
		return true;
	}
	if (date == order_date) {
		int option = _plMessageBoxYesNo(_T("���� ���� ������� �� ����� ���������.\n ����������, �� ��� ��� ��������� � ��� �� ����.\n ������������?"));
		if (option == IDNO) return false;
	}

	if (!act_reg_date.isNull() && date < act_reg_date) {
		error_str += error_str_prefix;
		error_str += _T(": �� ���� ���� ������ �� ���� ��������� ����\n");
	}

	return true;
}

//*****************************************************

bool CActDateValidator::validate(ImmutableString<Tchar> date_str, \
									const CDate &date, \
									Tstring &error_str, \
									const Tchar *error_str_prefix) const {

	assert(order_date_no != INVALID_FIELD_INDEX);

	auto rs = db_table->getResultSet();
	db_table->gotoCurrentRecord();

	bool is_null;
	CDate order_date = rs->getDate(order_date_no, is_null);

	CDate act_reg_date;
	if (act_reg_date_widget)
		act_reg_date.setDateGerman(act_reg_date_widget->GetLabel());
	else {
		assert(act_reg_dt_no != INVALID_FIELD_INDEX);
		act_reg_date = rs->getDate(act_reg_dt_no, is_null);
	}
	
	return CActDateValidatorBasic::validate(date_str, date, order_date, act_reg_date, \
											error_str, error_str_prefix);
}

//*****************************************************

bool CActDateValidatorNoDb::validate(ImmutableString<Tchar> date_str, \
									const CDate &date, \
									Tstring &error_str, \
									const Tchar *error_str_prefix) const {

	auto dt_str = order_date_widget->GetLabel();
	CDate order_date(dt_str, CDate::GERMAN_FORMAT);
	assert(!order_date.isNull());

	dt_str = act_reg_date_widget->GetLabel();
	CDate act_reg_date(dt_str, CDate::GERMAN_FORMAT);

	return CActDateValidatorBasic::validate(date_str, date, order_date, act_reg_date, \
											error_str, error_str_prefix);
}