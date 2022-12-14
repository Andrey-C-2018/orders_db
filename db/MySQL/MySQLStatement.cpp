#include "MySQLStatement.h"
#include "MySQLStatementException.h"
#include "MySQLResultSet.h"
#include "MySQLResultSetMetadata.h"
#include "MySQLVariant.h"
#include "MySQLStmtDataEx.h"
#include "MySQLBindingItem.h"

CMySQLStatement::CMySQLStatement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_) : \
												conn(conn_) {
	assert(conn_);
	assert(stmt_);
	stmt = std::make_shared<MySQLStmtDataEx>(stmt_);
	
	params.resize(stmt->getParamsCount());
}

CMySQLStatement::CMySQLStatement(CMySQLStatement &&obj) noexcept : \
												conn(std::move(obj.conn)), \
												stmt(std::move(obj.stmt)), \
												params(std::move(obj.params)) { }

CMySQLStatement &CMySQLStatement::operator=(CMySQLStatement &&obj) noexcept {

	conn = std::move(obj.conn);
	stmt = std::move(obj.stmt);
	params = std::move(obj.params);
	return *this;
}

void CMySQLStatement::reprepare(MYSQL_STMT *stmt_) {

	stmt->restoreStmt(stmt_);
}

size_t CMySQLStatement::getParamsCount() const {

	return stmt->getParamsCount();
}

void CMySQLStatement::assignParamBindingWithScalar(const size_t param_no, \
													CMySQLVariant &value) {

	MYSQL_BIND &param_binding = stmt->getBinding(param_no);

	param_binding.buffer_type = value.GetValueType();
	param_binding.buffer = value.GetValuePtr();
	param_binding.is_null = 0;
	param_binding.length = 0;
}

void CMySQLStatement::assignParamBindingWithVector(const size_t param_no, \
													MySQLBindingItem &param_item) {

	MYSQL_BIND &param_binding = stmt->getBinding(param_no);

	param_binding.buffer_length = (unsigned long)param_item.value.GetBufferSize();
	param_binding.buffer_type = param_item.value.GetValueType();
	param_binding.buffer = param_item.value.GetValuePtr();
	param_binding.is_null = 0;

	param_item.length = (unsigned long)param_item.value.GetValueLength();
	param_binding.length = &param_item.length;
}

void CMySQLStatement::bindValue(const size_t param_no, const int value) {

	assert(param_no < stmt->getParamsCount());
	if(params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_LONG);
	params[param_no].value.SetInt(value);

	assignParamBindingWithScalar(param_no, params[param_no].value);
}

void CMySQLStatement::bindValue(const size_t param_no, const char *value) {

	assert(param_no < stmt->getParamsCount());
	size_t len = strlen(value);
	if (params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, len);
	params[param_no].value.SetString(value);

	assignParamBindingWithVector(param_no, params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert(param_no < stmt->getParamsCount());
	size_t len = wcslen(value);
	if (params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, len);
	params[param_no].value.SetString(value);

	assignParamBindingWithVector(param_no, params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, \
								const ImmutableString<char> value) {

	assert(param_no < stmt->getParamsCount());
	if (params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, value.size);
	params[param_no].value.SetString(value.str, value.size);

	assignParamBindingWithVector(param_no, params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, \
								const ImmutableString<wchar_t> value) {

	assert(param_no < stmt->getParamsCount());
	if (params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_STRING, value.size);
	params[param_no].value.SetString(value.str, value.size);

	assignParamBindingWithVector(param_no, params[param_no]);
}

void CMySQLStatement::bindValue(const size_t param_no, const CDate &value) {

	assert(param_no < stmt->getParamsCount());
	if (params[param_no].value.IsNull())
		params[param_no].value = CMySQLVariant(MYSQL_TYPE_DATE);
	params[param_no].value.SetDate(value);

	assignParamBindingWithScalar(param_no, params[param_no].value);
}

void CMySQLStatement::bindValue(const size_t param_no, const CMySQLVariant &value) {

	assert(param_no < stmt->getParamsCount());
	params[param_no].value = value;

	if(value.IsVectorType())
		assignParamBindingWithVector(param_no, params[param_no]);
	else
		assignParamBindingWithScalar(param_no, params[param_no].value);
}

void CMySQLStatement::bindNull(const size_t param_no) {

	MYSQL_BIND &param_binding = stmt->getBinding(param_no);
	param_binding.buffer_length = 0;
	param_binding.buffer_type = MYSQL_TYPE_NULL;
	param_binding.buffer = nullptr;

	params[param_no].is_null = 1;
	params[param_no].length = 0;
	param_binding.is_null = &params[param_no].is_null;
	param_binding.length = &params[param_no].length;
}

std::shared_ptr<IDbResultSet> CMySQLStatement::exec() {

	stmt->applyParamsBindings();

	if (mysql_stmt_execute(stmt->stmt))
		throw CMySQLStatementException(stmt->stmt);

	if (!stmt->metadata)
		throw CMySQLStatementException(CMySQLStatementException::E_EXEC, \
										_T("exec() used instead of execScalar()"));

	return std::make_shared<CMySQLResultSet>(conn, stmt);
}

record_t CMySQLStatement::execScalar() {

	stmt->applyParamsBindings();

	if (mysql_stmt_execute(stmt->stmt))
		throw CMySQLStatementException(stmt->stmt);

	my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt->stmt);

	if (stmt->metadata)
		throw CMySQLStatementException(CMySQLStatementException::E_EXEC, \
										_T("execScalar() used instead of exec()"));

	return affected_rows;
}

std::shared_ptr<IDbResultSetMetadata> CMySQLStatement::getResultSetMetadata() {

	if (!stmt->metadata)
		throw CMySQLStatementException(CMySQLStatementException::E_METADATA, \
									_T("this prepared statement has no metadata"));

	return std::make_shared<CMySQLResultSetMetadata>(stmt);
}

CMySQLStatement::~CMySQLStatement() { }
