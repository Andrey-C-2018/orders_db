#include "PsychoInserter.h"
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/TransactionGuard.h>
#include <forms_common/BindersEx.h>

PsychoInserter::PsychoInserter() : people_ins_helper(PEOPLE_FIELDS_COUNT), \
									psycho_ins_helper(PSYCHO_FIELDS_COUNT) { }

void PsychoInserter::addFieldValueHolder(const char* field_name, XWidget* value_holder) {

	assert (field_name);
	assert (value_holder);
	assert (field_val_holders.find(field_name) == field_val_holders.cend());
	field_val_holders[field_name] = value_holder;
}

void PsychoInserter::prepare(std::shared_ptr<IDbConnection> conn_) {

	auto p_end = field_val_holders.cend();
	assert (field_val_holders.find("name") != p_end);
	assert(field_val_holders.find("address") != p_end);
	assert(field_val_holders.find("tel") != p_end);
	assert(field_val_holders.find("email") != p_end);
	assert(field_val_holders.find("bdate") != p_end);

	conn = conn_;
	std::string query = "INSERT INTO people(name, name_short, bdate) VALUES(";
	people_ins_helper.addBinder(0, _T("ПІБ"), \
							std::make_shared<CNameBinder>(field_val_holders["name"], false), 2);
	people_ins_helper.addBinder(2, _T("Дата народження"), \
							std::make_shared<UIDateInsertBinder>(field_val_holders["bdate"], false));

	people_ins_helper.buildQuery(query);
	query += ')';
	stmt_people = conn->PrepareQuery(query.c_str());

	query = "INSERT INTO psychologists(id_psycho, address, tel, email) VALUES(";
	psycho_ins_helper.defStaticInsertion(0, "LAST_INSERT_ID()");
	psycho_ins_helper.addBinder(1, _T("Адреса"), \
					std::make_shared<UITextInsertBinder>(field_val_holders["address"], false, true));
	psycho_ins_helper.addBinder(2, _T("Телефон"), \
					std::make_shared<UITextInsertBinder>(field_val_holders["tel"], false, true));
	psycho_ins_helper.addBinder(3, _T("E-mail"), \
					std::make_shared<CEmailBinder>(field_val_holders["email"], false));

	psycho_ins_helper.buildQuery(query);
	query += ')';
	stmt_psycho = conn->PrepareQuery(query.c_str());
}

bool PsychoInserter::insert() {

	Tstring error_str;
	if (!people_ins_helper.bind(stmt_people, 0, error_str)) {
		ErrorBox(error_str.c_str());
		return false;
	}
	if (!psycho_ins_helper.bind(stmt_psycho, 0, error_str)) {
		ErrorBox(error_str.c_str());
		return false;
	}

	{
		TransactionGuard guard(conn);
		stmt_people->execScalar();
		stmt_psycho->execScalar();
		guard.commit();
	}
	return true;
}

PsychoInserter::~PsychoInserter() {

	for (auto& p : field_val_holders) {
		auto widget = p.second;
		if (widget && !widget->IsCreated()) delete widget;
	}
}
