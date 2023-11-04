#pragma once
#include <string>
#include <map>
#include <db_ext/IDbInserter.h>
#include <db_ext/DbInsertHelper.h>
#include <xwindows/XWidget.h>

struct IDbStatement;

class PsychoInserter : public IDbInserter {
	enum {
		PEOPLE_FIELDS_COUNT = 3,
		PSYCHO_FIELDS_COUNT = 4
	};
	std::map<std::string, XWidget*> field_val_holders;

	CDbInsertHelper people_ins_helper, psycho_ins_helper;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt_psycho, stmt_people;

public:
	PsychoInserter();

	PsychoInserter(const PsychoInserter& obj) = delete;
	PsychoInserter(PsychoInserter&& obj) = delete;
	PsychoInserter& operator=(const PsychoInserter& obj) = delete;
	PsychoInserter& operator=(PsychoInserter&& obj) = delete;

	void addFieldValueHolder(const char *field_name, XWidget *value_holder);
	void prepare(std::shared_ptr<IDbConnection> conn) override;
	bool insert() override;

	virtual ~PsychoInserter();
};
