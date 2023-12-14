#pragma once
#include <string>
#include <map>
#include <db_ext/IDbInserter.h>
#include <db_ext/DbInsertHelper.h>
#include <xwindows/XWidget.h>

struct IDbStatement;

class InformersInserter : public IDbInserter {
	enum {
		INFORMERS_FIELDS_COUNT = 4
	};
	std::map<std::string, XWidget*> field_val_holders;

	CDbInsertHelper informers_ins_helper;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt_informers;

public:
	InformersInserter();

	InformersInserter(const InformersInserter& obj) = delete;
	InformersInserter(InformersInserter&& obj) = delete;
	InformersInserter& operator=(const InformersInserter& obj) = delete;
	InformersInserter& operator=(InformersInserter&& obj) = delete;

	void addFieldValueHolder(const char *field_name, XWidget *value_holder);
	void prepare(std::shared_ptr<IDbConnection> conn) override;
	bool insert() override;

	virtual ~InformersInserter();
};
