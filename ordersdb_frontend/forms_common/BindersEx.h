#pragma once
#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <db_ext/InsParamNoGuard.h>
#include <db_controls/InsertionBinders.h>

class CNameBinder : public CVisualInsertBinder {
	std::string surname, name_full, patr; // cache
public:
	CNameBinder(XWidget* adv_name_, bool free_widget) : \
		CVisualInsertBinder(adv_name_, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params& params, const Tchar* field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 2);

		auto err = _T("Невірний формат імені, має бути: '{Прізвище} {Імя} {По-батькові}'\n");

		size_t size;
		auto adv_name_str = widget->GetLabel(size);
		auto p = Tstrchr(adv_name_str, _T(' '));
		if (!p) { params.error_str += err; return true; }
		UCS16_ToUTF8(adv_name_str, (int)(p - adv_name_str), surname);

		while (*p == _T(' ')) ++p;

		auto p2 = Tstrchr(p, _T(' '));
		if (!p2) { params.error_str += err; return true; }
		UCS16_ToUTF8(p, (int)(p2 - p), name_full);

		while (*p2 == _T(' ')) ++p2;
		if (*p2 == _T('\0')) { params.error_str += err; return true; }
		UCS16_ToUTF8(p2, (int)(adv_name_str + size - p2), patr);

		surname += ' ';
		size_t adv_surname_size = surname.size();
		surname += name_full;
		surname += ' ';
		surname += patr;
		binding_target->bindValue(params.param_no, surname.c_str());

		surname.erase(adv_surname_size, surname.size() - adv_surname_size);
		size_t ln = getUnicodeSymbolLength(name_full[0]);
		for (size_t i = 0; i < ln && i < 4; ++i)
			surname += name_full[i];
		surname += ". ";
		ln = getUnicodeSymbolLength(patr[0]);
		for (size_t i = 0; i < ln && i < 4; ++i)
			surname += patr[i];
		surname += '.';
		binding_target->bindValue(params.param_no + 1, surname.c_str());

		return true;
	}

	virtual ~CNameBinder() { }
};

class CEmailBinder : public CVisualInsertBinder {
public:
	CEmailBinder(XWidget* email_holder, bool free_widget) : \
		CVisualInsertBinder(email_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params& params, const Tchar* field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto email_str = widget->GetLabel(size);

		if (!size) {
			binding_target->bindNull(params.param_no);
			return true;
		}

		auto p_dog = Tstrchr(email_str, _T('@'));
		if (!p_dog) {
			params.error_str += _T("в email-адресі відсутній символ '@'\n");
			return true;
		}
		binding_target->bindValue(params.param_no, email_str);
		return true;
	}

	virtual ~CEmailBinder() { }
};