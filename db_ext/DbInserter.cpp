#include <algorithm>
#include <db/IDbConnection.h>
#include "DbInserter.h"
#include <assert.h>

CDbInserterException::CDbInserterException(const int err_code, \
											const Tchar *err_descr) : \
											XException(err_code, err_descr) { }

CDbInserterException::CDbInserterException(const int err_code, Tstring &&err_descr) : \
											XException(err_code, std::move(err_descr)) { }

CDbInserterException::CDbInserterException(const CDbInserterException &obj) : \
											XException(obj) { }

CDbInserterException::~CDbInserterException() { }

//*****************************************************

CDbInserter::CDbInserter(const char *table_name, const size_t fields_count_) : \
							fields_count(fields_count_) {

	assert(table_name);
	this->table_name = table_name;
}

void CDbInserter::addBinder(const size_t field_no, \
							const Tchar *field_name, \
							std::shared_ptr<IInsertBinder> binder) {

	assert(binders.size() < fields_count);
	assert(field_no < fields_count);
	assert(field_name);

	Binders item;
	item.field_no = field_no;
	item.field_name = field_name;
	item.binder = binder;

	binders.emplace_back(item);
}

void CDbInserter::prepare(std::shared_ptr<IDbConnection> conn) {
	const size_t QUERY_RESERVED_SIZE = 50;

	assert(!binders.empty());
	assert(fields_count);
	std::string query;
	query.reserve(QUERY_RESERVED_SIZE);
	
	query = "INSERT INTO ";
	query += table_name;
	query += " VALUES(?";
	for (size_t i = 1; i < fields_count; ++i)
		query += ",?";
	query += ')';

	stmt = conn->PrepareQuery(query.c_str());

	std::sort(binders.begin(), binders.end());
}

void CDbInserter::insert() {

	IInsertBinder::Params params;
	params.param_no = 0;

	for (auto &item : binders) {
		auto param_no = params.param_no;
		auto err_str_size = params.error_str.size();

		item.binder->bind(stmt, params, item.field_name);
		assert(param_no < params.param_no);
		assert(params.param_no <= fields_count);
		assert(err_str_size <= params.error_str.size());
	}

	if (!params.error_str.empty())
		throw CDbInserterException(CDbInserterException::E_INSERT, \
			std::move(params.error_str));
	else
		stmt->execScalar();
}

CDbInserter::~CDbInserter() { }
