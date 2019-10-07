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
			error_str += _T("Ім'я акта не може бути довшим за ");
			Tchar buffer[3];
			error_str += XConv::ToString((size_t)MAX_ACT_NAME_LEN, buffer);
			error_str += _T(" симв.\n");
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
				(ch >= _T('А') && ch <= _T('Я')) || \
				ch == _T('.') || ch == _T('І'))) {

				error_str += _T("Невірне ім'я акта: ");
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

class CActDateValidator final {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	XWidget *act_reg_date_widget;
	size_t order_date_no, act_reg_dt_no;
public:
	CActDateValidator(std::shared_ptr<CDbTable> db_table_, \
						XWidget *act_reg_date_widget_) : \
			db_table(db_table_), act_reg_date_widget(act_reg_date_widget_), \
			order_date_no(INVALID_FIELD_INDEX), \
			act_reg_dt_no(INVALID_FIELD_INDEX) {
	
		const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
		order_date_no = meta_info.getFieldIndexByName("order_date");

		if (!act_reg_date_widget) 
			act_reg_dt_no = meta_info.getFieldIndexByName("act_reg_date");
	}

	CActDateValidator(const CActDateValidator &obj) = default;
	CActDateValidator(CActDateValidator &&obj) = default;
	CActDateValidator &operator=(const CActDateValidator &obj) = default;
	CActDateValidator &operator=(CActDateValidator &&obj) = default;

	inline bool validate(ImmutableString<Tchar> date_str, \
							const CDate &date, \
							Tstring &error_str, \
							const Tchar *error_str_prefix) const {

		assert(order_date_no != INVALID_FIELD_INDEX);
		if (date.isNull()) {

			error_str += error_str_prefix;
			error_str += _T(": невірний формат дати: ");
			error_str += date_str.str;
			error_str += _T('\n');
			return true;
		}
		if (date > CDate::now()) {
			int option = _plMessageBoxYesNo(_T("Дата акта в майбутньому. Продовжувати?"));
			if (option == IDNO) return false;
		}
		if (date < CDate(1, 1, 2013)) {
			error_str += error_str_prefix;
			error_str += _T(": не може бути меншою за 01.01.2013\n");
			return true;
		}

		auto rs = db_table->getResultSet();
		db_table->gotoCurrentRecord();

		bool is_null;
		CDate order_date = rs->getDate(order_date_no, is_null);
		assert(!is_null);
		if (date < order_date) {
			error_str += error_str_prefix;
			error_str += _T(": не може бути меншою за дату доручення\n");
			return true;
		}
		if (date == order_date) {
			int option = _plMessageBoxYesNo(_T("Дата акта співпадає із датою доручення.\n Малоімовірно, що акт був прийнятий в той же день.\n Продовжувати?"));
			if (option == IDNO) return false;
		}

		CDate act_reg_date;
		if (act_reg_date_widget) {
			act_reg_date.setDateGerman(act_reg_date_widget->GetLabel());
			is_null = act_reg_date.isNull();
		}
		else {
			assert(act_reg_dt_no != INVALID_FIELD_INDEX);
			act_reg_date = rs->getDate(act_reg_dt_no, is_null);
		}
		if (!is_null && date < act_reg_date) {
			error_str += error_str_prefix;
			error_str += _T(": не може бути меншою за дату прийняття акта\n");
		}

		return true;
	}
};
