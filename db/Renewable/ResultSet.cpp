#include <db/DbException.h>
#include "ResultSet.h"
#include "Statement.h"

ResultSet::ResultSet(StmtData data_, \
					std::shared_ptr<IDbResultSet> mysql_rs_) : \
				data(std::move(data_)), \
				mysql_rs(mysql_rs_) { }

ResultSet::ResultSet(ResultSet &&obj) noexcept : \
				data(std::move(obj.data)), \
				mysql_rs(std::move(obj.mysql_rs)) { }

ResultSet &ResultSet::operator=(ResultSet &&obj) noexcept {

	data = std::move(obj.data);
	mysql_rs = std::move(obj.mysql_rs);
	return *this;
}

bool ResultSet::empty() const {

	return mysql_rs->empty();
}

size_t ResultSet::getFieldsCount() const {

	return mysql_rs->getFieldsCount();
}

size_t ResultSet::getRecordsCount() const {

	return mysql_rs->getRecordsCount();
}

void ResultSet::gotoRecord(const size_t record) const {

	mysql_rs->gotoRecord(record);
}

int ResultSet::getInt(const size_t field, bool &is_null) const {

	return mysql_rs->getInt(field, is_null);
}

const char *ResultSet::getString(const size_t field) const {

	return mysql_rs->getString(field);
}

const wchar_t *ResultSet::getWString(const size_t field) const {

	return mysql_rs->getWString(field);
}

ImmutableString<char> ResultSet::getImmutableString(const size_t field) const {

	return mysql_rs->getImmutableString(field);
}

ImmutableString<wchar_t> ResultSet::getImmutableWString(const size_t field) const {

	return mysql_rs->getImmutableWString(field);
}

CDate ResultSet::getDate(const size_t field, bool &is_null) const {

	return mysql_rs->getDate(field, is_null);
}

void ResultSet::reload() {

	try {
		mysql_rs->reload();
	}
	catch (CDbException &e) {

		data.reprepareStmt(e);
		mysql_rs->reload();
	}
}
