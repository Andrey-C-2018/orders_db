#pragma once
#include <string>
#include <db/IDbStatement.h>
#include <db_ext/IBinder.h>

struct IDbConnection;

class CAdvocatInserter {
	enum {
		FIELDS_MAX_COUNT = 11
	};

	struct CAdvocatTableField {
		std::string name;
		std::shared_ptr<IBinder> binder;
	};

	CAdvocatTableField fields[FIELDS_MAX_COUNT];
	size_t initialized_fields_count;
	std::shared_ptr<IDbStatement> stmt;

public:
	CAdvocatInserter();

	CAdvocatInserter(const CAdvocatInserter &obj);
	CAdvocatInserter(CAdvocatInserter &&obj);
	CAdvocatInserter &operator=(const CAdvocatInserter &obj);
	CAdvocatInserter &operator=(CAdvocatInserter &&obj);

	void addInsertionBinder(const char *field_name, std::shared_ptr<IBinder> binder);
	void prepare(std::shared_ptr<IDbConnection> conn);
	inline void performInsertion();

	virtual ~CAdvocatInserter();
};

//**************************************************************

void CAdvocatInserter::performInsertion() {

	for (size_t i = 0; i < FIELDS_MAX_COUNT; ++i)
		fields[i].binder->bind(stmt, i);

	stmt->execScalar();
}

