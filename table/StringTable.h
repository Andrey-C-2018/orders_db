#pragma once
#include <vector>
#include <basic/Exception.h>
#include "ITable.h"

class CFieldsProperties : public IFieldsProperties {
	struct FieldProps {
		int field_size;
		Tstring field_name;
	};
	std::vector<FieldProps> fields_props;
public:
	CFieldsProperties();
	void AddField(const int field_size, const Tchar *field_name);

	int GetWidth(const size_t index) const override;
	ImmutableString<Tchar> GetName(const size_t index) const override;

	size_t size() const override;
	int GetSizesSumm() const override;

	virtual ~CFieldsProperties();
};

class CStringTable : public ITable {
	enum Defaults {
		DEF_ON_CHANGE_HANDLERS_COUNT = 5
	};
	typedef std::vector<Tstring> CTextVector;
	typedef std::vector<CTextVector> CTextTable;

	CTextTable text_table;
	std::shared_ptr<CFieldsProperties> fields_props;
	std::vector<ITableOnSizeChangedHandlerPtr> on_size_changed_handlers;
public:
	CStringTable();

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	std::shared_ptr<const IFieldsProperties> GetFieldsProperties() const override;
	void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) override;

	void AddField(const int field_size, const Tchar *field_name) override;
	void AddRecord() override;
	void SetFieldSize(const size_t field, const int field_size) override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, \
									const size_t record) const override;
	void SetCell(const size_t field, const size_t record, \
					const Tchar *value) override;

	virtual ~CStringTable();
};