#include "MySQLResultSet.h"
#include "../IDbBindingTarget.h"

CMySQLResultSetException::CMySQLResultSetException(const int err_code, \
													const Tchar *err_descr) : \
													CMySQLException(err_code, err_descr) { }

CMySQLResultSetException::CMySQLResultSetException(MYSQL *conn) : \
													CMySQLException(conn) { }

CMySQLResultSetException::CMySQLResultSetException(MYSQL_STMT *stmt) : \
													CMySQLException(stmt) { }

CMySQLResultSetException::CMySQLResultSetException(const CMySQLResultSetException &obj) : \
													CMySQLException(obj) { }

CMySQLResultSetException::~CMySQLResultSetException() { }

//*************************************************************

CMySQLResultSet::CMySQLResultSet(std::shared_ptr<MYSQL_STMT> stmt_, \
									std::shared_ptr<MYSQL_RES> metadata_) : \
								fields_count(0), records_count(0), \
								stmt(stmt_), metadata(metadata_), \
								curr_record((size_t)-1) {
	assert(stmt);
	assert(metadata);

	fields_count = mysql_num_fields(metadata.get());
	records_count = (size_t)mysql_stmt_num_rows(stmt.get());
	fields.reserve(fields_count);
	fields_bindings.reserve(fields_count);

	for (size_t i = 0; i < fields_count; ++i) {
		MYSQL_FIELD *field = mysql_fetch_field(metadata.get());
		assert(field);

		CMySQLBindingTarget rec;
		rec.value = CMySQLVariant(field->type, field->length);
		fields.emplace_back(std::move(rec));

		MYSQL_BIND field_binding;
		field_binding.buffer_type = field->type;
		if (rec.value.IsVectorType())
			field_binding.buffer_length = field->length;

		field_binding.buffer = fields[i].value.GetValuePtr();
		field_binding.length = &fields[i].length;
		field_binding.is_null = &fields[i].is_null;
		field_binding.error = &fields[i].error;

		fields_bindings.push_back(field_binding);
	}

	if (mysql_stmt_bind_result(stmt.get(), &fields_bindings[0]))
		throw CMySQLResultSetException(stmt.get());
}

CMySQLResultSet::CMySQLResultSet(CMySQLResultSet &&obj) : \
								fields_count(obj.fields_count), \
								records_count(obj.records_count), \
								stmt(std::move(obj.stmt)), \
								metadata(std::move(obj.metadata)), \
								curr_record(obj.curr_record), \
								fields(std::move(obj.fields)), \
								fields_bindings(std::move(obj.fields_bindings)) {

	assert(fields_count || !records_count);
	obj.fields_count = 0;
	obj.records_count = 0;
	obj.curr_record = (size_t)-1;
}

CMySQLResultSet &CMySQLResultSet::operator=(CMySQLResultSet &&obj) {

	assert(fields_count || !records_count);
	
	fields_count = obj.fields_count;
	records_count = obj.records_count;
	stmt = std::move(obj.stmt);
	metadata = std::move(obj.metadata);
	curr_record = obj.curr_record;
	
	obj.records_count = 0;
	obj.fields_count = 0;
	obj.curr_record = (size_t)-1;
	
	fields = std::move(obj.fields);
	fields_bindings = std::move(obj.fields_bindings);
	return *this;
}

bool CMySQLResultSet::empty() const {

	return !(records_count && fields_count);
}

size_t CMySQLResultSet::getFieldsCount() const {

	return fields_count;
}

size_t CMySQLResultSet::getRecordsCount() const {

	return records_count;
}

void CMySQLResultSet::gotoRecord(const size_t record) const {

	assert(record < records_count);
	if (curr_record == record) return;

	assert(stmt);
	mysql_stmt_data_seek(stmt.get(), record);

	volatile int success = mysql_stmt_fetch(stmt.get());
	if (success == MYSQL_DATA_TRUNCATED)
		throw CMySQLResultSetException(CMySQLResultSetException::E_TRUNC, \
										_T("fetched data was truncated"));
	assert(success == 0);

	curr_record = record;
}

int CMySQLResultSet::getInt(const size_t field, bool &is_null) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);

	is_null = fields[field].is_null > 0;
	return fields[field].value.GetInt();
}

const char *CMySQLResultSet::getString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	fields[field].value.UpdateLength(fields[field].length);

	bool is_null = fields[field].is_null > 0;
	return is_null ? nullptr : fields[field].value.GetString();
}

const wchar_t *CMySQLResultSet::getWString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	fields[field].value.UpdateLength(fields[field].length);

	bool is_null = fields[field].is_null > 0;
	return is_null ? nullptr : fields[field].value.GetWString();
}

ImmutableString<char> CMySQLResultSet::getImmutableString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	fields[field].value.UpdateLength(fields[field].length);

	if (!fields[field].is_null) {
		const char *str = fields[field].value.GetString();
		size_t len = fields[field].value.GetValueLength();
		return ImmutableString<char>(str, len);
	}

	return ImmutableString<char>(nullptr, 0);
}

ImmutableString<wchar_t> CMySQLResultSet::getImmutableWString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	fields[field].value.UpdateLength(fields[field].length);

	if (!fields[field].is_null) {
		const wchar_t *str = fields[field].value.GetWString();
		size_t len = fields[field].value.GetValueLength();
		return ImmutableString<wchar_t>(str, len);
	}

	return ImmutableString<wchar_t>(nullptr, 0);
}

CDate CMySQLResultSet::getDate(const size_t field, bool &is_null) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);

	is_null = fields[field].is_null > 0;
	return fields[field].value.GetDate();
}

void CMySQLResultSet::reload() {

	assert(stmt);
	assert(metadata);

	if (mysql_stmt_execute(stmt.get()))
		throw CMySQLResultSetException(stmt.get());

	if (mysql_stmt_store_result(stmt.get()))
		throw CMySQLResultSetException(stmt.get());

	records_count = (size_t)mysql_stmt_num_rows(stmt.get());
	if (records_count) {
		curr_record = (curr_record >= records_count) * (records_count - 1) + \
						(curr_record < records_count) * curr_record;
		mysql_stmt_data_seek(stmt.get(), curr_record);

		volatile int success = mysql_stmt_fetch(stmt.get());
		if (success == MYSQL_DATA_TRUNCATED)
			throw CMySQLResultSetException(CMySQLResultSetException::E_TRUNC, \
				_T("fetched data was truncated"));
		assert(success == 0);
	}
	else
		curr_record = (size_t)-1;
}

void CMySQLResultSet::Release() {

	curr_record = (size_t)-1;
	records_count = fields_count = 0;

	metadata.reset();
	stmt.reset();
}

CMySQLResultSet::~CMySQLResultSet() {

	Release();
}