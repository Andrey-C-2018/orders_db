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

void CDbInserter::defStaticInsertion(const size_t field_no, const char *expr) {

	assert(field_no < fields_count);
	static_insertions[field_no] = std::string(expr);
}

void CDbInserter::prepare(std::shared_ptr<IDbConnection> conn) {
	const size_t QUERY_RESERVED_SIZE = 70;
	auto appendToStr = [](std::string &str1, const char *str2, const size_t times) {

		for (size_t i = 0; i < times; ++i)	str1 += str2;
	};

	assert(!binders.empty());
	assert(fields_count);
	std::string query;
	query.reserve(QUERY_RESERVED_SIZE);
	
	query = "INSERT INTO ";
	query += table_name;
	query += " VALUES(";

	auto p = static_insertions.cbegin();
	size_t start = 0;
	while (p != static_insertions.cend()) {
			
		size_t end = p->first;
		appendToStr(query, "?,", end - start);
		query += p->second;
		query += ',';

		start = end + 1;
		++p;
	}

	appendToStr(query, "?,", fields_count - start);
	query[query.size() - 1] = ')';

	stmt = conn->PrepareQuery(query.c_str());

	std::sort(binders.begin(), binders.end());
}

bool CDbInserter::insert() {

	assert(stmt);
	IInsertBinder::Params params;
	params.param_no = 0;

	bool cancel = false;
	for (auto p_item = binders.begin(); !cancel && p_item != binders.end(); ++p_item) {
		auto param_no = params.param_no;
		auto err_str_size = params.error_str.size();

		cancel = !p_item->binder->bind(stmt, params, p_item->field_name);
		assert(param_no < params.param_no);
		assert(params.param_no <= fields_count);
		assert(err_str_size <= params.error_str.size());
	}

	if(cancel) return false;

	if (!params.error_str.empty())
		throw CDbInserterException(CDbInserterException::E_INSERT, \
									std::move(params.error_str));
	else
		stmt->execScalar();

	return true;
}

CDbInserter::~CDbInserter() { }
