#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/DbException.h>
#include <db_controls/DbComboBox.h>
#include <db_controls/InsertionBinders.h>
#include "InformersInserter.h"

InformersInserter::InformersInserter() : informers_ins_helper(INFORMERS_FIELDS_COUNT), \
                                            locations(nullptr) { }

void InformersInserter::addFieldValueHolder(const char* field_name, XWidget* value_holder) {

	assert (field_name);
	assert (value_holder);
	assert (field_val_holders.find(field_name) == field_val_holders.cend());
	field_val_holders[field_name] = value_holder;
}

void InformersInserter::prepare(std::shared_ptr<IDbConnection> conn_) {

	auto p_end = field_val_holders.cend();
	assert (field_val_holders.find("informer_name") != p_end);
	assert(field_val_holders.find("informer_full_name") != p_end);
	assert(locations);
	assert(field_val_holders.find("inf_class") != p_end);

	conn = conn_;
	std::string query = "INSERT INTO informers(informer_name, informer_full_name, id_location, inf_class) VALUES(";
	informers_ins_helper.addBinder(0, _T("Скорочена назва інформатора"), \
					std::make_shared<UITextInsertBinder>(field_val_holders["informer_name"], false));
	informers_ins_helper.addBinder(1, _T("Повна назва інформатора"), \
					std::make_shared<UITextInsertBinder>(field_val_holders["informer_full_name"], false));
    informers_ins_helper.addBinder(2, _T("Місцезнаходження"), \
				    std::make_shared<CDbComboBoxInsertBinder>(locations, false, false));
    informers_ins_helper.addBinder(3, _T("Тип інформатора"), \
					std::make_shared<UITextInsertBinder>(field_val_holders["inf_class"], false));

	informers_ins_helper.buildQuery(query);
	query += ')';
	stmt_informers = conn->PrepareQuery(query.c_str());
}

bool InformersInserter::insert() {

	try {
		Tstring error_str;
		if (!informers_ins_helper.bind(stmt_informers, 0, error_str)) {
			ErrorBox(error_str.c_str());
			return false;
		}

		stmt_informers->execScalar();
	}
	catch (CDbException& e) {

		ErrorBox(e.what());
		return false;
	}

	return true;
}

InformersInserter::~InformersInserter() {

	for (auto& p : field_val_holders) {
		auto widget = p.second;
		if (widget && !widget->IsCreated()) delete widget;
	}
    if (locations && !locations->IsCreated()) delete locations;
}
