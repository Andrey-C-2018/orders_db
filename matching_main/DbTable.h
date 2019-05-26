#pragma once
#include <vector>
#include "ITable.h"

struct IDbResultSet;
struct IDbResultSetMetadata;
struct IDbField;

class CDbRecord : public IRecord {
	std::shared_ptr<IDbResultSet> result_set;
	size_t record_no;

public:
	CDbRecord(std::shared_ptr<IDbResultSet> result_set_, const size_t record_no_);
	
	CDbRecord(const CDbRecord &obj) = default;
	CDbRecord(CDbRecord &&obj) = default;
	CDbRecord &operator=(const CDbRecord &obj) = default;
	CDbRecord &operator=(CDbRecord &&obj) = default;

	size_t size() const override;
	const char *getColValueAsCharArray(const size_t index) const override;

	virtual ~CDbRecord();
};

class CMySQLDbRecord : public IRecord {
	std::shared_ptr<IDbResultSet> result_set;
	std::shared_ptr<std::vector<std::shared_ptr<IDbField> > > fields;
	size_t record_no;
	mutable std::vector<char> buffer;

public:
	CMySQLDbRecord(std::shared_ptr<IDbResultSet> result_set_, \
					std::shared_ptr<std::vector<std::shared_ptr<IDbField> > > fields_, \
					const size_t record_no_);

	CMySQLDbRecord(const CMySQLDbRecord &obj) = default;
	CMySQLDbRecord(CMySQLDbRecord &&obj) = default;
	CMySQLDbRecord &operator=(const CMySQLDbRecord &obj) = default;
	CMySQLDbRecord &operator=(CMySQLDbRecord &&obj) = default;

	size_t size() const override;
	const char *getColValueAsCharArray(const size_t index) const override;

	virtual ~CMySQLDbRecord();
};

class CDbTable : public ITable {
	std::vector<std::string> fields_names;
protected:
	std::shared_ptr<IDbResultSet> result_set;
	std::shared_ptr<std::vector<std::shared_ptr<IDbField> > > fields;
public:
	CDbTable(std::shared_ptr<IDbResultSet> result_set_, \
				std::shared_ptr<IDbResultSetMetadata> metadata);

	CDbTable(const CDbTable &obj) = default;
	CDbTable(CDbTable &&obj) = default;
	CDbTable &operator=(const CDbTable &obj) = default;
	CDbTable &operator=(CDbTable &&obj) = default;

	size_t getColumnsCount() const override;
	size_t getColIndexByName(const char *col_name) const override;
	const char *getColName(const size_t index) const override;
	size_t getRecordsCount() const override;
	std::shared_ptr<const IRecord> getRecord(const size_t index) const override;
	std::shared_ptr<IRecord> getRecord(const size_t index) override;

	virtual ~CDbTable();
};

class CMySQLDbTable : public CDbTable {
public:
	CMySQLDbTable(std::shared_ptr<IDbResultSet> result_set_, \
					std::shared_ptr<IDbResultSetMetadata> metadata);

	CMySQLDbTable(const CMySQLDbTable &obj) = default;
	CMySQLDbTable(CMySQLDbTable &&obj) = default;
	CMySQLDbTable &operator=(const CMySQLDbTable &obj) = default;
	CMySQLDbTable &operator=(CMySQLDbTable &&obj) = default;

	std::shared_ptr<const IRecord> getRecord(const size_t index) const override;
	std::shared_ptr<IRecord> getRecord(const size_t index) override;

	virtual ~CMySQLDbTable();
};