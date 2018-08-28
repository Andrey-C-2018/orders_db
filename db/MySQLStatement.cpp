#include "MySQLStatement.h"
#include "MySQLResultSetMetadata.h"

CMySQLStatementException::CMySQLStatementException(const int err_code, \
													const Tchar *err_descr) : \
													CMySQLException(err_code, err_descr) { }

CMySQLStatementException::CMySQLStatementException(MYSQL *conn) : \
													CMySQLException(conn) { }

CMySQLStatementException::CMySQLStatementException(MYSQL_STMT *stmt) : \
													CMySQLException(stmt) { }

CMySQLStatementException::CMySQLStatementException(const CMySQLStatementException &obj) : \
													CMySQLException(obj) { }

CMySQLStatementException::~CMySQLStatementException() { }

//*************************************************************

CMySQLStatement::CMySQLStatement(MYSQL_STMT *stmt_) {

	assert(stmt_);
	stmt = std::shared_ptr<MYSQL_STMT>(stmt_, \
				[](MYSQL_STMT *stmt_) { mysql_stmt_close(stmt_); });

	params_count = mysql_stmt_param_count(stmt.get());

	if (params_count) {
		params.resize(params_count);

		MYSQL_BIND def_param_binding;
		memset((void *)&def_param_binding, 0, sizeof(MYSQL_BIND));

		params_bindings.insert(params_bindings.begin(), \
								params_count, def_param_binding);
	}
}

CMySQLStatement::CMySQLStatement(CMySQLStatement &&obj) : stmt(obj.stmt) {

	obj.stmt = nullptr;
}

CMySQLStatement &CMySQLStatement::operator=(CMySQLStatement &&obj) {

	stmt = obj.stmt;
	obj.stmt = nullptr;
	return *this;
}

void CMySQLStatement::assignParamBindingWithScalar(MYSQL_BIND &param_binding, \
													CMySQLVariant &value) {

	param_binding.buffer_type = value.GetValueType();
	param_binding.buffer = value.GetValuePtr();
	param_binding.is_null = 0;
	param_binding.length = 0;
}

void CMySQLStatement::assignParamBindingWithVector(MYSQL_BIND &param_binding, \
													CMySQLBindingTarget &param) {

	param_binding.buffer_length = (unsigned long)param.value.GetBufferSize();
	param_binding.buffer_type = param.value.GetValueType();
	param_binding.buffer = param.value.GetValuePtr();
	param_binding.is_null = 0;

	param.length = (unsigned long)param.value.GetValueLength();
	param_binding.length = &param.length;
}

void CMySQLStatement::bindValue(const size_t param_no, const int value) {

	assert(param_no < params_count);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_LONG);
	params[param_no].value.SetInt(value);

	assignParamBindingWithScalar(params_bindings[param_no], params[param_no].value);
}

void CMySQLStatement::bindValue(const size_t param_no, const char *value) {

	assert(param_no < params_count);
	size_t len = strlen(value);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, len);
	params[param_no].value.SetString(value);

	assignParamBindingWithVector(params_bindings[param_no], params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert(param_no < params_count);
	size_t len = wcslen(value);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, len);
	params[param_no].value.SetString(value);

	assignParamBindingWithVector(params_bindings[param_no], params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, \
								const ImmutableString<char> value) {

	assert(param_no < params_count);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, value.size);
	params[param_no].value.SetString(value.str, value.size);

	assignParamBindingWithVector(params_bindings[param_no], params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, \
								const ImmutableString<wchar_t> value) {

	assert(param_no < params_count);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, value.size);
	params[param_no].value.SetString(value.str, value.size);

	assignParamBindingWithVector(params_bindings[param_no], params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, const CDate &value) {

	assert(param_no < params_count);
	params[param_no].value = CMySQLVariant(MYSQL_TYPE_DATE);
	params[param_no].value.SetDate(value);

	assignParamBindingWithScalar(params_bindings[param_no], params[param_no].value);
}

void CMySQLStatement::bindValue(const size_t param_no, const CMySQLVariant &value) {

	assert(param_no < params_count);
	params[param_no].value = value;

	if(value.IsVectorType())
		assignParamBindingWithVector(params_bindings[param_no], params[param_no]);
	else
		assignParamBindingWithScalar(params_bindings[param_no], params[param_no].value);
}

std::shared_ptr<IDbResultSet> CMySQLStatement::exec() {

	if (params_count)
		mysql_stmt_bind_param(stmt.get(), &params_bindings[0]);

	if (mysql_stmt_execute(stmt.get()))
		throw CMySQLException(stmt.get());

	MYSQL_RES *metadata_ptr = mysql_stmt_result_metadata(stmt.get());
	if (!metadata_ptr)
		throw CMySQLStatementException(CMySQLStatementException::E_EXEC, \
										_T("exec() used instead of execScalar()"));

	metadata = std::shared_ptr<MYSQL_RES>(metadata_ptr, \
										[](MYSQL_RES *ptr) { mysql_free_result(ptr); });

	if (mysql_stmt_store_result(stmt.get()))
		throw CMySQLStatementException(stmt.get());

	return std::make_shared<CMySQLResultSet>(stmt, metadata);
}

record_t CMySQLStatement::execScalar() {

	if (params_count)
		mysql_stmt_bind_param(stmt.get(), &params_bindings[0]);

	if (mysql_stmt_execute(stmt.get()))
		throw CMySQLStatementException(stmt.get());

	if (mysql_stmt_result_metadata(stmt.get()))
		throw CMySQLStatementException(CMySQLStatementException::E_EXEC, \
										_T("execScalar() used instead of exec()"));

	return mysql_stmt_affected_rows(stmt.get());
}

std::shared_ptr<IDbResultSetMetadata> CMySQLStatement::getResultSetMetadata() {

	if (!metadata) {
		MYSQL_RES *metadata_ptr = mysql_stmt_result_metadata(stmt.get());
		if (!metadata_ptr)
			throw CMySQLStatementException(stmt.get());

		metadata = std::shared_ptr<MYSQL_RES>(metadata_ptr, \
			[](MYSQL_RES *ptr) { mysql_free_result(ptr); });
	}

	return std::make_shared<CMySQLResultSetMetadata>(metadata);
}

CMySQLStatement::~CMySQLStatement() { }
