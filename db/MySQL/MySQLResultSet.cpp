#include "../IDbBindingTarget.h"
#include "../IDbStaticResultSet.h"
#include "MySQLResultSet.h"
#include "MySQLStmtDataEx.h"
#include "MySQLBindingItem.h"

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

CMySQLResultSet::CMySQLResultSet(std::shared_ptr<MYSQL> conn_, \
								std::shared_ptr<const MySQLStmtDataEx> stmt_) : \
								fields_count(0), records_count(0), \
								conn(conn_), \
								stmt(stmt_), curr_record((size_t)-1) {
	assert(conn);
	assert(stmt);
	assert(stmt->stmt);
	assert(stmt->metadata);

	if (mysql_stmt_store_result(stmt->stmt))
		throw CMySQLResultSetException(stmt->stmt);

	fields_count = mysql_num_fields(stmt->metadata);
	assert(fields_count > 0);

	records_count = (size_t)mysql_stmt_num_rows(stmt->stmt);
	fields.reserve(fields_count);
	fields_bindings.reserve(fields_count);

	mysql_field_seek(stmt->metadata, (MYSQL_FIELD_OFFSET)0);
	for (size_t i = 0; i < fields_count; ++i) {
		MYSQL_FIELD *field_props = mysql_fetch_field(stmt->metadata);
		assert(field_props);

		fields.emplace_back();
		auto &field = fields[i];
		field.value = CMySQLVariant(field_props->type, field_props->length);

		fields_bindings.emplace_back();
		auto &field_binding = fields_bindings[i];

		field_binding.buffer_type = field_props->type;
		if (field.value.IsVectorType())
			field_binding.buffer_length = field_props->length;
		else
			field_binding.buffer_length = 0;

		field_binding.buffer = field.value.GetValuePtr();
		field_binding.length = &field.length;
		field_binding.is_null = &field.is_null;
		field_binding.error = &field.error;
	}

	rebind(); // TODO: use from stmt
}

void CMySQLResultSet::rebind() {

	if (mysql_stmt_bind_result(stmt->stmt, &fields_bindings[0]))
		throw CMySQLResultSetException(stmt->stmt);
}

CMySQLResultSet::CMySQLResultSet(CMySQLResultSet &&obj) noexcept : \
								fields_count(obj.fields_count), \
								records_count(obj.records_count), \
								conn(std::move(obj.conn)), \
								stmt(std::move(obj.stmt)), \
								curr_record(obj.curr_record), \
								fields(std::move(obj.fields)), \
								fields_bindings(std::move(obj.fields_bindings)) {

	assert(fields_count || !records_count);
	obj.fields_count = 0;
	obj.records_count = 0;
	obj.curr_record = (size_t)-1;
}

CMySQLResultSet &CMySQLResultSet::operator=(CMySQLResultSet &&obj) noexcept {

	assert(fields_count || !records_count);
	
	fields_count = obj.fields_count;
	records_count = obj.records_count;
	conn = std::move(obj.conn);
	stmt = std::move(obj.stmt);
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
	mysql_stmt_data_seek(stmt->stmt, record);

	volatile int success = mysql_stmt_fetch(stmt->stmt);
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
	assert(stmt->stmt);
	assert(stmt->metadata);

	stmt->applyParamsBindings();

	if (mysql_stmt_execute(stmt->stmt))
		throw CMySQLResultSetException(stmt->stmt);

	if (mysql_stmt_store_result(stmt->stmt))
		throw CMySQLResultSetException(stmt->stmt);

	records_count = (size_t)mysql_stmt_num_rows(stmt->stmt);
	if (records_count) {
		curr_record = (curr_record >= records_count) * (records_count - 1) + \
						(curr_record < records_count) * curr_record;
		mysql_stmt_data_seek(stmt->stmt, curr_record);

		volatile int success = mysql_stmt_fetch(stmt->stmt);
		if (success == MYSQL_DATA_TRUNCATED)
			throw CMySQLResultSetException(CMySQLResultSetException::E_TRUNC, \
										_T("fetched data was truncated"));
		assert(success == 0);
	}
	else
		curr_record = (size_t)-1;
}

void CMySQLResultSet::upload(IDbStaticResultSet &static_rs) const {

	assert(stmt);
	static_rs.init(fields_count, records_count);
	size_t field_id_rec = static_rs.getFieldRecId();

	for (curr_record = 0; curr_record < records_count; curr_record++) {

		mysql_stmt_data_seek(stmt->stmt, curr_record);

		volatile int success = mysql_stmt_fetch(stmt->stmt);
		if (success == MYSQL_DATA_TRUNCATED)
			throw CMySQLResultSetException(CMySQLResultSetException::E_TRUNC, \
										_T("fetched data was truncated"));
		assert(success == 0);
		assert(!fields[field_id_rec].is_null);
		int id = fields[field_id_rec].value.GetInt();
		static_rs.gotoRecord(curr_record, id);

		for (size_t j = 0; j < fields_count; j++) {

			bool is_null = fields[j].is_null > 0;
			Variant v;
			if (!is_null) {
				if (fields[j].value.IsVectorType()) 
					fields[j].value.UpdateLength(fields[j].length);
				v = fields[j].value.toStdVariant();
			}
			static_rs.setValue(j, std::move(v));
		}
	}
}

CMySQLResultSet::~CMySQLResultSet() { }
