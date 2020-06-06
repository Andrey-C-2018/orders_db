#pragma once
#include <basic/XConv.h>
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>
#include <xwindows/XWidget.h>
#include "Validators.h"

enum {
	MAX_INT_FIELD_LEN = getDigitsCountOfType<int>() + 1
};

class CFeeBinder : public CVisualInsertBinder {
	bool allow_empty;
public:
	CFeeBinder(XWidget *fee_holder, bool free_widget, bool allow_empty_) : \
		CVisualInsertBinder(fee_holder, free_widget), allow_empty(allow_empty_) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto fee_str = widget->GetLabel(size);

		if (!size) {
			if (allow_empty) {
				binding_target->bindValue(params.param_no, "0.0");
				return true;
			}

			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			return true;
		}

		size_t i = 0;
		while (i < size && \
			((fee_str[i] >= _T('0') && fee_str[i] <= _T('9')) || \
				fee_str[i] == _T('.') || fee_str[i] == _T(','))) ++i;

		if (i < size) {
			params.error_str += field_name;
			params.error_str += _T(": невірний формат: ");
			params.error_str += fee_str;
			params.error_str += _T('\n');
			return true;
		}

		auto p = Tstrchr(fee_str, _T('.'));
		const Tchar *p2 = nullptr;
		if (!p) {
			p2 = Tstrchr(fee_str, _T(','));
			p = p2;
		}

		if (p) size = (p - fee_str);
		if (size > 6) {
			params.error_str += field_name;
			params.error_str += _T(": не може перевищувати 1 млн. грн.\n");
			return true;
		}

		if (p2) {
			Tchar fee[10];
			Tstrncpy(fee, fee_str, 6);
			size_t fee_size = Tstrlen(fee);
			fee[fee_size] = _T('.');
			++fee_size;
			Tstrncpy(fee + fee_size, fee_str + fee_size, 2);
			fee[fee_size + 2] = _T('\0');
			binding_target->bindValue(params.param_no, fee);
		}
		else
			binding_target->bindValue(params.param_no, fee_str);

		return true;
	}

	virtual ~CFeeBinder() { }
};

class CActNameBinder : public CVisualInsertBinder {

	CActNameValidator validator;
public:
	CActNameBinder(XWidget *act_name_holder, bool free_widget) : \
		CVisualInsertBinder(act_name_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto act_str = widget->GetLabel(size);

		size_t err_str_size = params.error_str.size();
		if (!validator.validate(ImmutableString<Tchar>(act_str, size), params.error_str))
			return false;

		if (err_str_size == params.error_str.size()) {

			const Tchar *act_name = validator.getActName();
			if (!act_name || (act_name && act_name[0] == _T('\0')))
				binding_target->bindNull(params.param_no);
			else
				binding_target->bindValue(params.param_no, act_name);
		}

		return true;
	}

	virtual ~CActNameBinder() { }
};

class CActDateBinder : public CVisualInsertBinder {

	CActDateValidator validator;
public:
	CActDateBinder(std::shared_ptr<CDbTable> db_table_, \
					XWidget *act_date_holder, bool free_widget, \
					XWidget *act_reg_date_holder) : \
		CVisualInsertBinder(act_date_holder, free_widget), \
		validator(db_table_, act_reg_date_holder) {	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			binding_target->bindNull(params.param_no);
			return true;
		}

		size_t err_str_size = params.error_str.size();
		CDate date(date_str, CDate::GERMAN_FORMAT);

		if (!validator.validate(ImmutableString<Tchar>(date_str, size), \
			date, params.error_str, field_name)) return false;

		if (err_str_size == params.error_str.size())
			binding_target->bindValue(params.param_no, date);

		return true;
	}

	virtual ~CActDateBinder() { }
};

class CActRegDateBinder : public CVisualInsertBinder {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	size_t order_date_no;
public:
	CActRegDateBinder(std::shared_ptr<CDbTable> db_table_, \
						XWidget *act_date_holder, bool free_widget) : \
				CVisualInsertBinder(act_date_holder, free_widget), \
				db_table(db_table_), \
				order_date_no(INVALID_FIELD_INDEX) {

		const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
		order_date_no = meta_info.getFieldIndexByName("order_date");
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			binding_target->bindNull(params.param_no);
			return true;
		}

		CDate date(date_str, CDate::GERMAN_FORMAT);
		if (date.isNull()) {

			params.error_str += field_name;
			params.error_str += _T(": невірний формат дати\n");
			return true;
		}
		if (date > CDate::now()) {
			int option = _plMessageBoxYesNo(_T("Дата прийняття акта в майбутньому. Продовжувати?"));
			if (option == IDNO) return false;
		}

		auto rs = db_table->getResultSet();
		db_table->gotoCurrentRecord();

		bool is_null;
		CDate order_date = rs->getDate(order_date_no, is_null);
		assert(!is_null);
		if (date < order_date) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути меншою за дату доручення\n");
			return true;
		}
		if (date == order_date) {
			int option = _plMessageBoxYesNo(_T("Дата прийняття акта співпадає із датою доручення. \nПродовжувати?"));
			if (option == IDNO) return false;
		}

		binding_target->bindValue(params.param_no, date);
		return true;
	}

	virtual ~CActRegDateBinder() { }
};

class CQaParamBinder : public CVisualInsertBinder {
public:
	CQaParamBinder(XWidget *qa_param_holder, bool free_widget) : \
		CVisualInsertBinder(qa_param_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto qa_param_str = widget->GetLabel(size);
		if (size == 0) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			return true;
		}

		if (qa_param_str[0] != _T('0') && qa_param_str[0] != _T('1')) {
			params.error_str += field_name;
			params.error_str += _T(": допустимі значення - 0 або 1\n");
			return true;
		}

		binding_target->bindValue(params.param_no, (int)(qa_param_str[0] - _T('0')));
		return true;
	}

	virtual ~CQaParamBinder() { }
};

class CQaKoefBinder : public CVisualInsertBinder {
public:
	CQaKoefBinder(XWidget *qa_koef_holder, bool free_widget) : \
		CVisualInsertBinder(qa_koef_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto qa_koef_str = widget->GetLabel(size);
		if (size == 0) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			return true;
		}

		Tchar *end = nullptr;
		float value = wcstof(qa_koef_str, &end);

		if (end - qa_koef_str > 4 || \
			!(value <= 1e-6 || (value - 0.25F) <= 1e-6 || \
			(value - 0.5F) <= 1e-6 || (value - 0.75F) <= 1e-6 || \
				(value - 1.0F) <= 1e-6)) {

			params.error_str += field_name;
			params.error_str += _T(": допустимі значення - 0, 0.25, 0.5, 0.75 або 1\n");
			return true;
		}

		binding_target->bindValue(params.param_no, qa_koef_str);
		return true;
	}

	virtual ~CQaKoefBinder() { }
};

//*****************************************************

inline const Tchar *getIntFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
										const size_t field_no, \
										Tchar(&buffer)[MAX_INT_FIELD_LEN]) {

	bool is_null;
	int value = rs->getInt(field_no, is_null);

	return is_null ? _T("") : XConv::ToString(value, buffer);
}

inline ImmutableString<char> getFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
												const size_t field_no, \
												char type_hint) {

	return rs->getImmutableString(field_no);
}

inline ImmutableString<wchar_t> getFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
												const size_t field_no, \
												wchar_t type_hint) {

	return rs->getImmutableWString(field_no);
}

inline void setIntWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name, \
								Tchar(&buffer)[MAX_INT_FIELD_LEN]) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	widget->SetLabel(getIntFieldAsString(rs, field_no, buffer));
}

inline void setStrWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	auto label = getFieldAsString(rs, field_no, Tchar());
	if (label.isNull()) {
		widget->SetLabel(_T(""));
		return;
	}
	widget->SetLabel(label.str, label.size);
}

inline void setDateWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name, \
								Tchar *buffer) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	bool is_null;
	CDate dt = rs->getDate(field_no, is_null);

	if (is_null) {
		widget->SetLabel(_T(""));
		return;
	}
	dt.toStringGerman(buffer);
	widget->SetLabel(buffer);
}
