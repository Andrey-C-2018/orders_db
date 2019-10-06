#pragma once
#include <vector>
#include <map>
#include <basic/Exception.h>
#include <db/IDbStatement.h>
#include "IInsertBinder.h"

class CDbInserterException : public XException {
public:
	enum {
		E_INSERT = 1
	};
	CDbInserterException(const int err_code, const Tchar *err_descr);
	CDbInserterException(const int err_code, Tstring &&err_descr);
	CDbInserterException(const CDbInserterException &obj);
	CDbInserterException(CDbInserterException &&obj) = default;
	~CDbInserterException();
};

struct IDbConnection;

class CDbInserter {
	const size_t fields_count;
	std::string table_name;
	std::shared_ptr<IDbStatement> stmt;

	struct Binders {
		size_t field_no;
		const Tchar *field_name;
		std::shared_ptr<IInsertBinder> binder;

		inline bool operator<(const Binders &obj) const { return field_no < obj.field_no; }
	};
	std::vector<Binders> binders;
	std::map<size_t, std::string> static_insertions;

public:
	CDbInserter(const char *table_name, const size_t fields_count_);

	CDbInserter(const CDbInserter &obj) = default;
	CDbInserter(CDbInserter &&obj) = default;
	CDbInserter &operator=(const CDbInserter &obj) = default;
	CDbInserter &operator=(CDbInserter &&obj) = default;

	inline const std::string &getTableName() const { return table_name; }
	void addBinder(const size_t field_no, const Tchar *field_name, \
							std::shared_ptr<IInsertBinder> binder);
	void defStaticInsertion(const size_t field_no, const char *expr);

	virtual void prepare(std::shared_ptr<IDbConnection> conn);
	virtual void insert();

	virtual ~CDbInserter();
};

class CInsParamNoGuard final {
	size_t &param_no;
	const size_t inc;
public:
	inline CInsParamNoGuard(size_t &param_no_, const size_t inc_) noexcept : \
		param_no(param_no_), inc(inc_) {

		assert(inc > 0);
	}

	CInsParamNoGuard(const CInsParamNoGuard &obj) = delete;
	CInsParamNoGuard(CInsParamNoGuard &&obj) = default;
	CInsParamNoGuard &operator=(const CInsParamNoGuard &obj) = delete;
	CInsParamNoGuard &operator=(CInsParamNoGuard &&obj) = default;

	~CInsParamNoGuard() {

		param_no += inc;
	}
};