#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <db/IDbConnection.h>
#include <db/IDbResultSet.h>
#include <db/IDbStatement.h>
#include <db/DbException.h>
#include <db/TransactionGuard.h>
#include <db_ext/InsParamNoGuard.h>
#include <xwindows/XWidget.h>
#include <xwindows/XComboBox.h>
#include <db_controls/DbComboBox.h>
#include "AdvocatInserter.h"

class AutoincrIdBinder : public IInsertBinder {
	std::shared_ptr<IDbConnection> conn;
public:

	explicit AutoincrIdBinder(std::shared_ptr<IDbConnection> conn_) : conn(conn_) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);
        int id = (int)conn->getLastInsertedId();
		binding_target->bindValue(params.param_no, id);
		return true;
	}

	virtual ~AutoincrIdBinder() { }
};

class CAdvNameBinder : public CVisualInsertBinder {
	std::string adv_surname, adv_name_full, adv_patr; // cache
public:
	CAdvNameBinder(XWidget *adv_name_, bool free_widget) : \
						CVisualInsertBinder(adv_name_, free_widget)	{ }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 2);

		auto err = _T("Невірний формат імені адвоката, має бути: '{Прізвище} {Імя} {По-батькові}'\n");

		size_t size;
		auto adv_name_str = widget->GetLabel(size);
		auto p = Tstrchr(adv_name_str, _T(' '));
		if (!p) { params.error_str += err; return true;	}
		UCS16_ToUTF8(adv_name_str, (int)(p - adv_name_str), adv_surname);

		while (*p == _T(' ')) ++p;

		auto p2 = Tstrchr(p, _T(' '));
		if (!p2) { params.error_str += err; return true; }
		UCS16_ToUTF8(p, (int)(p2 - p), adv_name_full);

		while (*p2 == _T(' ')) ++p2;
		if (*p2 == _T('\0')) { params.error_str += err; return true; }
		UCS16_ToUTF8(p2, (int)(adv_name_str + size - p2), adv_patr);

		adv_surname += ' ';
		size_t adv_surname_size = adv_surname.size();
		adv_surname += adv_name_full;
		adv_surname += ' ';
		adv_surname += adv_patr;
		binding_target->bindValue(params.param_no, adv_surname.c_str());

		adv_surname.erase(adv_surname_size, adv_surname.size() - adv_surname_size);
		size_t ln = getUnicodeSymbolLength(adv_name_full[0]);
		for (size_t i = 0; i < ln && i < 4; ++i)
			adv_surname += adv_name_full[i];
		adv_surname += ". ";
		ln = getUnicodeSymbolLength(adv_patr[0]);
		for (size_t i = 0; i < ln && i < 4; ++i)
			adv_surname += adv_patr[i];
		adv_surname += '.';
		binding_target->bindValue(params.param_no + 1, adv_surname.c_str());

		return true;
	}

	virtual ~CAdvNameBinder() { }
};

class CPostIndexBinder : public CVisualInsertBinder {
public:
	CPostIndexBinder(XWidget *post_index_holder, bool free_widget) : \
						CVisualInsertBinder(post_index_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto post_index_str = widget->GetLabel(size);

		if (size < 3 || size > 5) {
			params.error_str += _T("Кількість цифр у поштовому індексі - від 3 до 5\n");
			return true;
		}

		binding_target->bindValue(params.param_no, post_index_str);
		return true;
	}

	virtual ~CPostIndexBinder() { }
};

class CAdvOrgBinder : public IInsertBinder {
	XWidget *org_name;
	XComboBox *org_type;
	bool free_org_name, free_org_type;

public:
	CAdvOrgBinder(XWidget *org_name_, XComboBox *org_type_, \
					bool free_org_name_, bool free_org_type_) : \
				org_name(org_name_), org_type(org_type_), \
				free_org_name(free_org_name_), free_org_type(free_org_type_) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 2);

		size_t name_size;
		auto org_name_str = org_name->GetLabel(name_size);
		size_t type_size;
		auto org_type_str = org_type->GetLabel(type_size);

		if (Tstrncmp(org_type_str, _T("ІНД"), type_size)) {
			if (Tstrncmp(org_type_str, _T("АО"), type_size) && \
				Tstrncmp(org_type_str, _T("АБ"), type_size)) {

				params.error_str += _T("Невірне знаяення типу організації: ");
				params.error_str += org_type_str;
				params.error_str += _T("Має бути: АО, АБ або ІНД\n");
				return true;
			}

			if (org_name_str && org_name_str[0] == _T('\0')) {
				params.error_str += _T("Назва адвокатського бюро або об'єднання не може бути порожньою\n");
				return true;
			}
		}
		else {
			org_name_str = _T("");
			name_size = 0;
		}

		std::string utf8_buffer;
		if (name_size) {
			UCS16_ToUTF8(org_name_str, (int)name_size, utf8_buffer);
			binding_target->bindValue(params.param_no, utf8_buffer.c_str());
		}
		else
			binding_target->bindNull(params.param_no);

		if (!type_size) {
			org_type_str = _T("ІНД");
			type_size = 3;
		}
		UCS16_ToUTF8(org_type_str, (int)type_size, utf8_buffer);
		binding_target->bindValue(params.param_no + 1, utf8_buffer.c_str());
		return true;
	}

	virtual ~CAdvOrgBinder() {
	
		if (free_org_name && org_name && !org_name->IsCreated()) delete org_name;
		if (free_org_type && org_type && !org_type->IsCreated()) delete org_type;
	}
};

//*****************************************************

CAdvocatInserter::CAdvocatInserter() : people_ins_helper(PEOPLE_FIELDS_COUNT),\
                        adv_ins_helper(ADV_FIELDS_COUNT),\
						adv_name(nullptr), license_no(nullptr),\
						license_date(nullptr), examiner(nullptr), post_index(nullptr),\
						address(nullptr), tel(nullptr), email(nullptr),\
						adv_bdate(nullptr), district(nullptr), org_name(nullptr), \
						org_type(nullptr) { }

void CAdvocatInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(adv_name);
	assert(license_no);
	assert(license_date);
	assert(examiner);
	assert(post_index);
	assert(address);
	assert(tel);
	assert(email);
	assert(adv_bdate);
	assert(district);
	assert(org_name);
	assert(org_type);

    std::string query = "INSERT INTO people(name, name_short, bdate) VALUES(";
    people_ins_helper.addBinder(0, _T("ПІБ"), \
						std::make_shared<CAdvNameBinder>(adv_name, false), 2);
    people_ins_helper.addBinder(2, _T("Дата народження"), \
						std::make_shared<UIDateInsertBinder>(adv_bdate, false));

    people_ins_helper.buildQuery(query);
    query += ')';
    stmt_people = conn->PrepareQuery(query.c_str());

    query = "INSERT INTO advocats VALUES(";
	adv_ins_helper.addBinder(0, _T("ID"), \
					std::make_shared<AutoincrIdBinder>(conn));
	adv_ins_helper.addBinder(1, _T("Номер свідоцтва"), \
							std::make_shared<UITextInsertBinder>(license_no, false));
	adv_ins_helper.addBinder(2, _T("Дата свідоцтва"), \
						std::make_shared<UIDateInsertBinder>(license_date, false));
	adv_ins_helper.addBinder(3, _T("Екзаменатор"), \
				std::make_shared<CDbComboBoxInsertBinder>(examiner, false, false));
	adv_ins_helper.addBinder(4, _T("Поштовий індекс"), \
						std::make_shared<CPostIndexBinder>(post_index, false));
	adv_ins_helper.addBinder(5, _T("Адреса"), \
						std::make_shared<UITextInsertBinder>(address, false));
	adv_ins_helper.addBinder(6, _T("Телефон"), \
						std::make_shared<UITextInsertBinder>(tel, false));
	adv_ins_helper.addBinder(7, _T("E-mail"), \
							std::make_shared<UITextInsertBinder>(email, false));
	adv_ins_helper.addBinder(8, _T("Основний район роботи"), \
				std::make_shared<CDbComboBoxInsertBinder>(district, false, false));
	adv_ins_helper.addBinder(9, _T("Організація"), \
				std::make_shared<CAdvOrgBinder>(org_name, org_type, false, false), 2);

	adv_ins_helper.buildQuery(query);
	query += ')';
    stmt_adv = conn->PrepareQuery(query.c_str());
}

bool CAdvocatInserter::insert() {

	Tstring error_str;
    if (!people_ins_helper.bind(stmt_people, 0, error_str)) {
        ErrorBox(error_str.c_str());
        return false;
    }
	if (!adv_ins_helper.bind(stmt_adv, 0, error_str)) {
		ErrorBox(error_str.c_str());
		return false;
	}

	{
        TransactionGuard guard(conn);
        stmt_people->execScalar();
		stmt_adv->execScalar();
        // FIXME: bind the autoincr ID
        guard.commit();
	}
	return true;
}

CAdvocatInserter::~CAdvocatInserter() {

	if (org_type && !org_type->IsCreated()) delete org_type;
	if (org_name && !org_name->IsCreated()) delete org_name;
	if (district && !district->IsCreated()) delete district;
	if (adv_bdate && !adv_bdate->IsCreated()) delete adv_bdate;
	if (email && !email->IsCreated()) delete email;
	if (tel && !tel->IsCreated()) delete tel;
	if (address && !address->IsCreated()) delete address;
	if (post_index && !post_index->IsCreated()) delete post_index;

	if (examiner && !examiner->IsCreated()) delete examiner;
	if (license_date && !license_date->IsCreated()) delete license_date;
	if (license_no && !license_no->IsCreated()) delete license_no;
	if (adv_name && !adv_name->IsCreated()) delete adv_name;
}
